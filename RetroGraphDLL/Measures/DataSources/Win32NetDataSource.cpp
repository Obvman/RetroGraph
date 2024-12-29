module RG.Measures.DataSources:Win32NetDataSource;

import RG.Core;

import "CSTDHeaderUnit.h";
import "RGAssert.h";
import "WindowsNetworkHeaderUnit.h";

#pragma comment(lib, "Iphlpapi.lib")

namespace rg {

Win32NetDataSource::Win32NetDataSource(std::chrono::milliseconds pingFrequency, const std::string& pingServer)
    : m_table{ getIfTable() }
    , m_bestIfaceIndex{ getBestAdapterIndex() }
    , m_adapterRow{ getBestAdapterRow(m_bestIfaceIndex) }
    , m_bestAdapter{ determineBestAdapter() }
    , m_dnsIP{ determineDNSIP() }
    , m_hostname{ determineHostname() }
    , m_mainAdapterName{ wstrToStr(m_adapterRow->Description) }
    , m_mainAdapterMAC{ determineMAC(m_bestAdapter) }
    , m_mainAdapterIP{ m_bestAdapter.IpAddressList.IpAddress.String }
    , m_connectionChecker{ pingFrequency, pingServer } {}

bool Win32NetDataSource::updateBestAdapter() {
    int bestIfaceIndex{ getBestAdapterIndex() };

    if (bestIfaceIndex != m_bestIfaceIndex) {
        m_bestIfaceIndex = bestIfaceIndex;
        m_table = getIfTable();

        m_adapterRow = getBestAdapterRow(bestIfaceIndex);

        m_bestAdapter = determineBestAdapter();

        m_dnsIP = determineDNSIP();
        m_hostname = determineHostname();
        m_mainAdapterName = wstrToStr(m_adapterRow->Description);
        m_mainAdapterMAC = determineMAC(m_bestAdapter);
        m_mainAdapterIP = m_bestAdapter.IpAddressList.IpAddress.String;

        return true;
    }

    return false;
}

void Win32NetDataSource::updateNetTraffic() {
    const auto oldDown{ m_adapterRow->InOctets };
    const auto oldUp{ m_adapterRow->OutOctets };

    RGVERIFY(GetIfEntry2(m_adapterRow) == NO_ERROR, "GetIfEntry2 failed");
    m_downBytes = m_adapterRow->InOctets - oldDown;
    m_upBytes = m_adapterRow->OutOctets - oldUp;
}

_MIB_IF_TABLE2* Win32NetDataSource::getIfTable() const {
    _MIB_IF_TABLE2* table;
    RGVERIFY(GetIfTable2(&table) == NO_ERROR, "GetIfTable failed");
    return table;
}

int Win32NetDataSource::getBestAdapterIndex() const {
    DWORD bestIfaceIndex{ 0 };
    RGVERIFY(GetBestInterface(INADDR_ANY, &bestIfaceIndex) == NO_ERROR, "Failed to get best interface");
    return bestIfaceIndex;
}

_MIB_IF_ROW2* Win32NetDataSource::getBestAdapterRow(int bestIfaceIndex) const {
    // Find and keep track of the entry for the most appropriate local network interface
    for (auto i = size_t{ 0U }; i < m_table->NumEntries; ++i) {
        if (static_cast<int>(m_table->Table[i].InterfaceIndex) == bestIfaceIndex) {
            std::wcout << L"Using interface " << m_table->Table[i].Description << '\n';
            return &m_table->Table[i];
        }
    }

    RGERROR("Failed to find adapter");
    return nullptr;
}

IP_ADAPTER_INFO Win32NetDataSource::determineBestAdapter() const {
    auto bufferLen = ULONG{ sizeof(IP_ADAPTER_INFO) };
    std::vector<std::byte> adapterInfoBuffer(bufferLen);

    // Make initial call to find the required buffer size, then reallocate
    // buffer and make second call to fill buffer
    if (GetAdaptersInfo(reinterpret_cast<IP_ADAPTER_INFO*>(adapterInfoBuffer.data()), &bufferLen) ==
        ERROR_BUFFER_OVERFLOW) {
        adapterInfoBuffer.resize(bufferLen);
    }

    // Get the MAC address and LAN IP address of the main adapter
    if (GetAdaptersInfo(reinterpret_cast<IP_ADAPTER_INFO*>(adapterInfoBuffer.data()), &bufferLen) == NO_ERROR) {
        for (auto currAdapter{ reinterpret_cast<const IP_ADAPTER_INFO*>(adapterInfoBuffer.data()) }; currAdapter;
             currAdapter = currAdapter->Next) {
            // Convert adapter description from wchar_t for string comparison
            size_t dummy;
            char adapterDesc[128];
            wcstombs_s(&dummy, adapterDesc, sizeof(adapterDesc), m_adapterRow->Description, sizeof(adapterDesc) - 1);
            if (strcmp(currAdapter->Description, adapterDesc) == 0) {
                return *currAdapter;
            }
        }
    } else {
        RGERROR("GetAdaptersInfo failed");
    }

    return {};
}

std::string Win32NetDataSource::determineMAC(const IP_ADAPTER_INFO& adapter) const {
    // Convert the MAC address into a string
    std::stringstream macStream;
    macStream << std::hex << std::uppercase << std::setfill('0');
    for (auto i = size_t{ 0U }; i < adapter.AddressLength; i++) {
        macStream << std::setw(2) << static_cast<int>(adapter.Address[i]);
        if (i != (adapter.AddressLength - 1)) {
            macStream << '-';
        }
    }

    return macStream.str();
}

void Win32NetDataSource::getFixedInfoBuffer(std::vector<std::byte>& fixedInfoBuffer) const {
    auto bufferLen = ULONG{ sizeof(FIXED_INFO) };
    fixedInfoBuffer.resize(bufferLen);

    // Make an initial call to GetNetworkParams to get the necessary size into the bufferLen variable
    if (GetNetworkParams(reinterpret_cast<FIXED_INFO*>(fixedInfoBuffer.data()), &bufferLen) == ERROR_BUFFER_OVERFLOW) {
        fixedInfoBuffer.resize(bufferLen);
    }

    RGVERIFY(GetNetworkParams(reinterpret_cast<FIXED_INFO*>(fixedInfoBuffer.data()), &bufferLen) == NO_ERROR,
             "Failed to get network parameters");
}

std::string Win32NetDataSource::determineDNSIP() const {
    std::vector<std::byte> fixedInfoBuffer;
    getFixedInfoBuffer(fixedInfoBuffer);
    const auto* fixedInfo{ reinterpret_cast<const FIXED_INFO*>(fixedInfoBuffer.data()) };

    std::string dnsIP{ fixedInfo->DnsServerList.IpAddress.String };
    if (fixedInfo->DnsServerList.Next) {
        dnsIP += ", " + std::string{ fixedInfo->DnsServerList.Next->IpAddress.String };
    }
    return dnsIP;
}

std::string Win32NetDataSource::determineHostname() const {
    std::vector<std::byte> fixedInfoBuffer;
    getFixedInfoBuffer(fixedInfoBuffer);
    const auto* fixedInfo{ reinterpret_cast<const FIXED_INFO*>(fixedInfoBuffer.data()) };
    return fixedInfo->HostName;
}

} // namespace rg
