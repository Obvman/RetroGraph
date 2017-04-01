#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x601

#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <Windows.h>
#include <ws2def.h>
#include <ws2ipdef.h>
#include <Iphlpapi.h>
#include <wininet.h>
#include <icmpapi.h>
#include <Ws2tcpip.h>

#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Ws2_32.lib")

// Placing here avoids some nasty header conflicts
#include "../headers/NetMeasure.h"

namespace rg {

NetMeasure::NetMeasure(const UserSettings& settings) :
    m_pingServer{ settings.getPingServer() },
    m_pingFreqMs{ settings.getPingFreq() },
    dataSize{ settings.getNetUsageSamples() } {

    // Fill data vectors with default values
    m_downBytes.assign(dataSize, 0U);
    m_upBytes.assign(dataSize, 0U);

    if (GetIfTable2(&m_table) != NO_ERROR) {
        fatalMessageBox("GetIfTable failed");
    }

    /* Find and keep track of the entry for the most appropriate local network
     * interface
     */
    DWORD bestIfaceIndex;
    if (GetBestInterface(INADDR_ANY, &bestIfaceIndex) != NO_ERROR) {
        fatalMessageBox("Failed to get best interface");
    }

    // Get the adapter struct that corresponds to the hard-coded adapter name
    for (auto i = size_t{ 0U }; i < m_table->NumEntries; ++i) {
        if (m_table->Table[i].InterfaceIndex == bestIfaceIndex) {
            std::wcout << L"Using interface " << m_table->Table[i].Description << '\n';
            m_adapterEntry = &m_table->Table[i];
            break;
        }
    }

    if (!m_adapterEntry) {
        fatalMessageBox("Failed to find adapter");
    }

    getDNSAndHostname();
    getMACAndLocalIP();

    // Start thread that periodically checks connection to internet.
    m_netConnectionThread = std::thread{ [this]() {
        while (true) {
            // !! is to convert Win32 BOOL to bool without compiler warning :/
            setIsConnected(!!InternetCheckConnectionA(
                m_pingServer.c_str(), FLAG_ICC_FORCE_CONNECTION, 0));
            Sleep(1000 * m_pingFreqMs);
        }
    }};
    m_netConnectionThread.detach();
}

void NetMeasure::getMACAndLocalIP() {
    auto pAdapterInfo{ (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO)) };
    auto ulOutBufLen = ULONG{ sizeof(IP_ADAPTER_INFO) };

    // Make initial call to find the required buffer size, then reallocate
    // buffer and make second call to fill buffer
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
    }
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) != NO_ERROR) {
        fatalMessageBox("Failed to get adapters info");
    }

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
        fatalMessageBox("GetAdaptersInfo failed");
    }
    free(pAdapterInfo);
}

void NetMeasure::getDNSAndHostname() {
    // Get DNS and Hostname
    auto pFixedInfo{ (FIXED_INFO*)malloc(sizeof(FIXED_INFO)) };
    if (!pFixedInfo) {
        printf("Error allocating memory needed to call GetNetworkParams\n");
    }
    auto ulOutBufLen = ULONG{ sizeof(FIXED_INFO) };

    // Make an initial call to GetNetworkParams to get
    // the necessary size into the ulOutBufLen variable
    if (GetNetworkParams(pFixedInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(pFixedInfo);
        pFixedInfo = (FIXED_INFO*)malloc(ulOutBufLen);
        if (!pFixedInfo) {
            printf("Error allocating memory needed to call GetNetworkParams\n");
        }
    } else {
        std::cout << "Failed to get FIXED_INFO buffer size\n";
    }

    if (GetNetworkParams(pFixedInfo, &ulOutBufLen) != NO_ERROR) {
        fatalMessageBox("Failed to get network parameters");
    }

    m_DNSIP = std::string{ pFixedInfo->DnsServerList.IpAddress.String };
    m_hostname = std::string{ pFixedInfo->HostName };

    free(pFixedInfo);
}

void NetMeasure::update(uint32_t ticks) {
    if ((ticks % (ticksPerSecond / 2)) == 0) {
        // Check if the best network interface has changed and update to the new
        // one if so.
        if ((ticks % (ticksPerSecond * 30)) == 0) {
            DWORD bestIfaceIndex;
            if (GetBestInterface(INADDR_ANY, &bestIfaceIndex) != NO_ERROR) {
                fatalMessageBox("Failed to get best interface");
            }

            if (bestIfaceIndex != m_adapterEntry->InterfaceIndex) {
                m_adapterEntry = &(m_table->Table[bestIfaceIndex]);
            }
        }

        const auto oldDown{ m_adapterEntry->InOctets };
        const auto oldUp{ m_adapterEntry->OutOctets };

        if (GetIfEntry2(m_adapterEntry) != NO_ERROR) {
            fatalMessageBox("GetIfEntry2 failed");
        }

        m_downBytes[0] = m_adapterEntry->InOctets - oldDown;
        std::rotate(m_downBytes.begin(), m_downBytes.begin() + 1, m_downBytes.end());
        m_downMaxVal = *std::max_element(m_downBytes.cbegin(), m_downBytes.cend());

        m_upBytes[0] = m_adapterEntry->OutOctets - oldUp;
        std::rotate(m_upBytes.begin(), m_upBytes.begin() + 1, m_upBytes.end());
        m_upMaxVal = *std::max_element(m_upBytes.cbegin(), m_upBytes.cend());
    }
}

bool NetMeasure::isConnected() const {
    return m_isConnected.load();
}

void NetMeasure::setIsConnected(bool b) {
    m_isConnected.store(b);
}

}
