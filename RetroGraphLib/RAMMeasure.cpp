#include "stdafx.h"

#include "RAMMeasure.h"

#include <iostream>
#include <string>

// #include <GL/freeglut.h>

#include "colors.h"
#include "utils.h"
#include "UserSettings.h"

namespace rg {

RAMMeasure::RAMMeasure() :
    Measure{ 2 },
    dataSize{ UserSettings::inst().getVal<int>("Widgets-Graphs-RAM.NumUsageSamples") } {

    // Fill the memory stat struct with system information
    m_memStatus.dwLength = sizeof(m_memStatus);
    GlobalMemoryStatusEx(&m_memStatus);

    m_usageData.assign(dataSize, 0.0f);
}

void RAMMeasure::update(int) {
    GlobalMemoryStatusEx(&m_memStatus);

    // Add value to the list of load values and shift the list left
    m_usageData.front() = getLoadPercentagef();
    std::rotate(m_usageData.begin(), m_usageData.begin() + 1,
                m_usageData.end());
}

void RAMMeasure::refreshSettings() {
    const int newDataSize{ UserSettings::inst().getVal<int>("Widgets-Graphs-RAM.NumUsageSamples") };
    if (dataSize == newDataSize)
        return;

    m_usageData.assign(newDataSize, 0.0f);
    dataSize = newDataSize;
}

bool RAMMeasure::shouldUpdate(int ticks) const {
    return ticksMatchRate(ticks, m_updateRates.front());
}

float RAMMeasure::getLoadPercentagef() const {
    return (static_cast<float>(getUsedPhysicalB()) /
            static_cast<float>(getTotalPhysicalB()));
}



}
