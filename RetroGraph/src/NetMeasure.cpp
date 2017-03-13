#include "../headers/NetMeasure.h"

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

#include "../headers/utils.h"

#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Wininet.lib")

namespace rg {

NetMeasure::NetMeasure(const UserSettings& settings) :
    m_adapterEntry{ nullptr },
    m_mainAdapter{ settings.getNetAdapterName() },
    m_DNSIP{ "0.0.0.0" },
    m_hostname{ "" },
    m_mainAdapterMAC{ "00-00-00-00-00-00" },
    m_mainAdapterIP{ "0.0.0.0" },
    m_downMaxVal{ 10U * GB },
    m_upMaxVal{ 10U * GB },
    dataSize{ 40U } {
}

NetMeasure::~NetMeasure() {
}

void NetMeasure::init() {
    // Fill data vectors with default values
    m_downBytes.assign(dataSize, 0U);
    m_upBytes.assign(dataSize, 0U);

    MIB_IF_TABLE2* table{ nullptr };
    if (GetIfTable2(&table) != NO_ERROR) {
        fatalMessageBox("GetIfTable failed");
    }

    // Get the adapter struct that corresponds to the hard-coded adapter name
    for (auto i = size_t{ 0U }; i < table->NumEntries; ++i) {
        if (wcscmp(table->Table[i].Description, strToWstr(m_mainAdapter).c_str()) == 0) {
            m_adapterEntry = &table->Table[i];
            break;
        }
    }

    if (!m_adapterEntry) {
        fatalMessageBox("Failed to find adapter");
    }

    getNetStats();
}

void NetMeasure::getNetStats() {
    { // Get DNS and Hostname
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

    { // Get MAC and LAN IP for main adapter
        auto pAdapterInfo{ (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO)) };
        auto ulOutBufLen = ULONG{ sizeof(IP_ADAPTER_INFO) };

        if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
            free(pAdapterInfo);
            pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
        }

        if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) != NO_ERROR) {
            fatalMessageBox("Failed to get adapters info");
        }

        // Get the MAC address and LAN IP address of the main adapter
        if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR) {
            auto currAdapter{ pAdapterInfo };
            while (currAdapter) {
                if (m_mainAdapter == currAdapter->Description) {
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
                currAdapter = currAdapter->Next;
            }
        } else {
            fatalMessageBox("GetAdaptersInfo failed");
        }
        free(pAdapterInfo);
    }
}

void NetMeasure::update(uint32_t ticks) {
    if ((ticks % (ticksPerSecond / 2)) == 0) {
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

        if ((ticks % (ticksPerSecond * 10)) == 0) {
            static auto ping{ []() {
                InternetCheckConnectionA("http://www.google.com/",
                                         FLAG_ICC_FORCE_CONNECTION, 0);
            }};
            // Ping google to check if network is working
            /*printTimeToExecute("Ping", []() {
                if (InternetCheckConnectionA("http://www.google.com/", FLAG_ICC_FORCE_CONNECTION, 0)) {
                    std::cout << "Connected to internet\n";
                }
            });*/
            printTimeToExecute("Ping", ping);
        }
    }
}


}