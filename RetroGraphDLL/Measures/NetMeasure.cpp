module Measures.NetMeasure;

import UserSettings;

import "RGAssert.h";
import "WindowsNetworkHeaderUnit.h";

#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Ws2_32.lib")

namespace rg {

NetMeasure::NetMeasure()
    : m_pingServer{ UserSettings::inst().getVal<std::string>("Network.PingServer") }
    , m_pingFreqSec{ UserSettings::inst().getVal<int>("Network.PingFrequency") }
    , dataSize{ UserSettings::inst().getVal<int, size_t>("Widgets-NetGraph.NumUsageSamples") } {

    // Fill data vectors with default values
    m_downBytes.assign(dataSize, 0);
    m_upBytes.assign(dataSize, 0);

    RGVERIFY(GetIfTable2(&m_table) == NO_ERROR, "GetIfTable failed");

    /* Find and keep track of the entry for the most appropriate local network
     * interface
     */
    DWORD bestIfaceIndex;
    RGVERIFY(GetBestInterface(INADDR_ANY, &bestIfaceIndex) == NO_ERROR, "Failed to get best interface");

    // Get the adapter struct that corresponds to the hard-coded adapter name
    for (auto i = size_t{ 0U }; i < m_table->NumEntries; ++i) {
        if (m_table->Table[i].InterfaceIndex == bestIfaceIndex) {
            std::wcout << L"Using interface " << m_table->Table[i].Description << '\n';
            m_adapterEntry = &m_table->Table[i];
            break;
        }
    }

    RGASSERT(m_adapterEntry, "Failed to find adapter");

    getDNSAndHostname();
    getMACAndLocalIP();

    startNetworkThread();

    UserSettings::inst().registerRefreshProc(
        [&]() {
            destroyNetworkThread();

            m_pingServer = UserSettings::inst().getVal<std::string>("Network.PingServer");
            m_pingFreqSec = UserSettings::inst().getVal<int>("Network.PingFrequency");

            const size_t newDataSize = UserSettings::inst().getVal<int, size_t>("Widgets-NetGraph.NumUsageSamples");
            if (dataSize != newDataSize) {
                m_downBytes.assign(newDataSize, 0U);
                m_upBytes.assign(newDataSize, 0U);
                dataSize = newDataSize;
            }

            startNetworkThread();
        });
}

NetMeasure::~NetMeasure() {
    destroyNetworkThread();
}

void NetMeasure::getMACAndLocalIP() {
    auto pAdapterInfo{ static_cast<IP_ADAPTER_INFO*>(malloc(sizeof(IP_ADAPTER_INFO))) };
    auto ulOutBufLen = ULONG{ sizeof(IP_ADAPTER_INFO) };

    // Make initial call to find the required buffer size, then reallocate
    // buffer and make second call to fill buffer
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
    }

    RGVERIFY(GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR, "Failed to get adapters info");

    // Get the MAC address and LAN IP address of the main adapter
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR) {
        for (auto currAdapter{ pAdapterInfo };
             currAdapter;
             currAdapter = currAdapter->Next) {

            // Convert adapter description from wchar_t for string comparison
            size_t dummy;
            char adapterDesc[128];
            wcstombs_s(&dummy, adapterDesc, sizeof(adapterDesc),
                       m_adapterEntry->Description, sizeof(adapterDesc) - 1);
            if (strcmp(currAdapter->Description, adapterDesc) == 0) {

                // Convert the MAC address into a string
                std::stringstream macStream;
                macStream << std::hex << std::uppercase << std::setfill('0');
                for (auto i = size_t{ 0U }; i < currAdapter->AddressLength; i++) {
                    if (i == (currAdapter->AddressLength - 1)) {
                        macStream << std::setw(2)
                            << static_cast<int>(currAdapter->Address[i]);
                    } else {
                        macStream << std::setw(2)
                            << static_cast<int>(currAdapter->Address[i])
                            << '-';
                    }
                }

                m_mainAdapterMAC = macStream.str();
                m_mainAdapterIP = std::string{
                    currAdapter->IpAddressList.IpAddress.String
                };
                break;
            }
        }
    } else {
        RGERROR("GetAdaptersInfo failed");
    }
    free(pAdapterInfo);
}

void NetMeasure::getDNSAndHostname() {
    // Get DNS and Hostname
    auto * pFixedInfo{ static_cast<FIXED_INFO*>(malloc(sizeof(FIXED_INFO))) };
    RGASSERT(pFixedInfo, "Error allocating memory needed to call GetNetworkParams\n");

    auto ulOutBufLen = ULONG{ sizeof(FIXED_INFO) };

    // Make an initial call to GetNetworkParams to get
    // the necessary size into the ulOutBufLen variable
    if (GetNetworkParams(pFixedInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(pFixedInfo);
        pFixedInfo = static_cast<FIXED_INFO*>(malloc(ulOutBufLen));
        RGASSERT(pFixedInfo, "Error allocating memory needed to call GetNetworkParams\n");
    }

    RGVERIFY(GetNetworkParams(pFixedInfo, &ulOutBufLen) == NO_ERROR, "Failed to get network parameters");

    m_DNSIP = std::string{ pFixedInfo->DnsServerList.IpAddress.String };
    if (pFixedInfo->DnsServerList.Next) {
        m_DNSIP += ", " + std::string{pFixedInfo->DnsServerList.Next->IpAddress.String};
    }
    
    m_hostname = std::string{ pFixedInfo->HostName };

    free(pFixedInfo);
}

void NetMeasure::update(int ticks) {
    // Check if the best network interface has changed and update to the new
    // one if so.
    if (ticksMatchSeconds(ticks, 30)) {
        DWORD bestIfaceIndex;
        if (GetBestInterface(INADDR_ANY, &bestIfaceIndex) != NO_ERROR) {
            RGERROR("Failed to get best interface");
            return;
        }

        if (bestIfaceIndex != m_adapterEntry->InterfaceIndex) {
            m_adapterEntry = &(m_table->Table[bestIfaceIndex]);
        }
    }

    const auto oldDown{ m_adapterEntry->InOctets };
    const auto oldUp{ m_adapterEntry->OutOctets };

    RGVERIFY(GetIfEntry2(m_adapterEntry) == NO_ERROR, "GetIfEntry2 failed");

    m_downBytes[0] = m_adapterEntry->InOctets - oldDown;
    std::rotate(m_downBytes.begin(), m_downBytes.begin() + 1,
                m_downBytes.end());
    m_downMaxVal = *std::max_element(m_downBytes.cbegin(),
                                     m_downBytes.cend());

    m_upBytes[0] = m_adapterEntry->OutOctets - oldUp;
    std::rotate(m_upBytes.begin(), m_upBytes.begin() + 1, m_upBytes.end());
    m_upMaxVal = *std::max_element(m_upBytes.cbegin(), m_upBytes.cend());
}

bool NetMeasure::isConnected() const {
    return m_isConnected.load();
}

void NetMeasure::setIsConnected(bool b) {
    m_isConnected.store(b);
}

void NetMeasure::startNetworkThread() {
    // Start thread that periodically checks connection to internet.
    using namespace std::chrono_literals;

    m_threadRunning.store(true);
    m_netConnectionThread = std::thread{ [this]() {
        while (m_threadRunning.load()) {
            std::unique_lock<std::mutex> lg{ m_netConnectionMutex };
            setIsConnected(static_cast<bool>(
                InternetCheckConnectionA(m_pingServer.c_str(), FLAG_ICC_FORCE_CONNECTION, 0)
            ));

            m_netConnectionCV.wait_for(lg, 1000ms * m_pingFreqSec, [&]() { return !m_threadRunning.load(); });
        }
    }};

}

void NetMeasure::destroyNetworkThread() {
    // End the background thread
    m_threadRunning.store(false);
    m_netConnectionCV.notify_all();
    m_netConnectionThread.join();
}

} // namespace rg