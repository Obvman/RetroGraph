module RG.Measures:NetMeasure;

import RG.Core;

import "RGAssert.h";
import "WindowsNetworkHeaderUnit.h";

#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Ws2_32.lib")

namespace rg {

NetMeasure::NetMeasure()
    : Measure{ seconds{ 1 } }
    , m_pingServer{ UserSettings::inst().getVal<std::string>("Network.PingServer") }
    , m_pingFreqSec{ UserSettings::inst().getVal<int>("Network.PingFrequency") }
    , m_configRefreshedHandle{
        UserSettings::inst().configRefreshed.attach(
            [&]() {
                destroyNetworkThread();

                m_pingServer = UserSettings::inst().getVal<std::string>("Network.PingServer");
                m_pingFreqSec = UserSettings::inst().getVal<int>("Network.PingFrequency");

                startNetworkThread();
            })
    } {

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
}

NetMeasure::~NetMeasure() {
    destroyNetworkThread();
    UserSettings::inst().configRefreshed.detach(m_configRefreshedHandle);
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

bool NetMeasure::updateInternal() {
    // Check if the best network interface has changed and update to the new
    // one if so.
    static Timer updateBestInterfaceTime{ std::chrono::seconds{ 30 } };
    if (updateBestInterfaceTime.hasElapsed()) {
        DWORD bestIfaceIndex;
        if (GetBestInterface(INADDR_ANY, &bestIfaceIndex) != NO_ERROR) {
            RGERROR("Failed to get best interface");
            return false;
        }

        if (bestIfaceIndex != m_adapterEntry->InterfaceIndex) {
            m_adapterEntry = &(m_table->Table[bestIfaceIndex]);
        }

        updateBestInterfaceTime.restart();
    }

    const auto oldDown{ m_adapterEntry->InOctets };
    const auto oldUp{ m_adapterEntry->OutOctets };

    RGVERIFY(GetIfEntry2(m_adapterEntry) == NO_ERROR, "GetIfEntry2 failed");

    onDownBytes.raise(m_adapterEntry->InOctets - oldDown);
    onUpBytes.raise(m_adapterEntry->OutOctets - oldUp);
    return true;
}

bool NetMeasure::isConnected() const {
    return m_isConnected.load();
}

void NetMeasure::setIsConnected(bool b) {
    if (isConnected() != b) {
        m_isConnected.store(b);
        onConnectionStatusChanged.raise(b);
    }
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
