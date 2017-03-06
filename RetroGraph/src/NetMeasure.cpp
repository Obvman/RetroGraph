#include "../headers/NetMeasure.h"

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x601

#include <iostream>
#include <algorithm>
#include <Windows.h>
#include <ws2def.h>
#include <ws2ipdef.h>
#include <Iphlpapi.h>

#include "../headers/utils.h"

#pragma comment(lib, "Iphlpapi.lib")

namespace rg {

NetMeasure::NetMeasure() :
    m_adapterEntry{ nullptr },
    m_downMaxVal{ 10U * GB },
    m_upMaxVal{ 10U * GB },
    dataSize{ 40U } {

    m_downBytes.assign(dataSize, 0U);
    m_upBytes.assign(dataSize, 0U);

    MIB_IF_TABLE2* table{ nullptr };
    if (GetIfTable2(&table) != NO_ERROR) {
        fatalMessageBox("GetIfTable failed");
    }

    // Get the adapter struct that corresponds to the hard-coded adapter name
    for (auto i{ 0U }; i < table->NumEntries; ++i) {
        if (wcscmp(table->Table[i].Description, myAdapter) == 0) {
            m_adapterEntry = &table->Table[i];
            break;
        }
    }

    if (!m_adapterEntry) {
        fatalMessageBox("Failed to find adapter");
    }
}

NetMeasure::~NetMeasure() {
}

void NetMeasure::update() {
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