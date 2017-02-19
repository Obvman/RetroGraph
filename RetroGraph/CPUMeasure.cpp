#include "CPUMeasure.h"

#include <algorithm>
#include <Windows.h>

static unsigned long long FileTimeToInt64(const FILETIME & ft) {return (((unsigned long long)(ft.dwHighDateTime))<<32)|((unsigned long long)ft.dwLowDateTime);}

namespace rg {

CPUMeasure::CPUMeasure() :
    dataSize{ 10U },
    m_usageData{ } {

    // fill vector with default values
    m_usageData.assign(dataSize, 0.0f);
}


CPUMeasure::~CPUMeasure() {
}

float CPUMeasure::getCPULoad() {
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        auto load = calculateCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime));

        // Add to the usageData vector by overwriting the oldest value and
        // shifting the elements in the vector

        // option 1: shift elements left and add to the end afterwards
        // option 2: add then use std::rotate
        m_usageData[0] = load;
        std::rotate(m_usageData.begin(), m_usageData.begin() + 1, m_usageData.end());

        return load;
    } else {
        return -1.0f;
    }

    return GetSystemTimes(&idleTime, &kernelTime, &userTime) ?
        calculateCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime)) : -1.0f;
}

float CPUMeasure::calculateCPULoad(uint64_t idleTicks, uint64_t totalTicks) {
    static uint64_t prevTotalTicks{ 0U };
    static uint64_t prevIdleTicks{ 0U };

    uint64_t totalTicksSinceLastTime = totalTicks - prevTotalTicks;
    uint64_t idleTicksSinceLastTime = idleTicks - prevIdleTicks;

    float ret = 1.0f - ((totalTicksSinceLastTime > 0) ? (static_cast<float>(idleTicksSinceLastTime))/totalTicksSinceLastTime : 0);

    prevTotalTicks = totalTicks;
    prevIdleTicks = idleTicks;

    return ret;
}

}

