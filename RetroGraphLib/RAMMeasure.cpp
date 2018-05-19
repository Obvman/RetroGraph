#include "stdafx.h"

#include "RAMMeasure.h"

#include <iostream>
#include <string>

// #include <GL/freeglut.h>

#include "colors.h"
#include "UserSettings.h"

namespace rg {

RAMMeasure::RAMMeasure() :
    dataSize{ std::get<uint32_t>(
                  UserSettings::inst().getVal("Widgets-Graphs-RAM.NumUsageSamples")
              ) } {

    // Fill the memory stat struct with system information
    m_memStatus.dwLength = sizeof(m_memStatus);
    GlobalMemoryStatusEx(&m_memStatus);

    m_usageData.assign(dataSize, 0.0f);
}

void RAMMeasure::update(uint32_t ticks) {
    if ((ticks % (ticksPerSecond / 2)) == 0) {
        GlobalMemoryStatusEx(&m_memStatus);

        // Add value to the list of load values and shift the list left
        m_usageData[0] = getLoadPercentagef();
        std::rotate(m_usageData.begin(), m_usageData.begin() + 1,
                    m_usageData.end());
    }
}

float RAMMeasure::getLoadPercentagef() const {
    return (static_cast<float>(getUsedPhysicalB()) /
            static_cast<float>(getTotalPhysicalB()));
}

}
