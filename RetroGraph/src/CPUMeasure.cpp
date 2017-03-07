#include "../headers/CPUMeasure.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <time.h>
#include <Windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include "../headers/colors.h"
#include "../headers/utils.h"

static unsigned long long FileTimeToInt64(const FILETIME & ft) {return (((unsigned long long)(ft.dwHighDateTime))<<32)|((unsigned long long)ft.dwLowDateTime);}

namespace rg {

CPUMeasure::CPUMeasure() :
    m_coreTempPlugin{},
    dataSize{ 40U },
    m_usageData{ },
    m_uptime{ std::chrono::milliseconds{GetTickCount64()} },
    m_cpuName{ } {

    // fill vector with default values
    m_usageData.assign(dataSize, 0.0f);
}

CPUMeasure::~CPUMeasure() {
}

void CPUMeasure::update() {
    m_coreTempPlugin.update();
    m_uptime = std::chrono::milliseconds(GetTickCount64());

    // Fill CPU name if CoreTemp interfacing was successful
    if (m_cpuName.size() == 0 && m_coreTempPlugin.getCoreTempInfoSuccess()) {
        m_cpuName = "CPU: ";
        m_cpuName.append(m_coreTempPlugin.getCPUName());
    }

    getCPULoad();
}

float CPUMeasure::getCPULoad() {
    FILETIME idleTime;
    FILETIME kernelTime;
    FILETIME userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return -1.0f;
    }
    const auto load{ calculateCPULoad(FileTimeToInt64(idleTime),
                     FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime))
    };

    // Add to the usageData vector by overwriting the oldest value and
    // shifting the elements in the vector
    m_usageData[0] = load;
    std::rotate(m_usageData.begin(), m_usageData.begin() + 1, m_usageData.end());
    return load;
}
std::string CPUMeasure::getUptimeStr() const {
    const auto uptimeS = (m_uptime / 1000) % 60;
    const auto uptimeM = (m_uptime / (60 * 1000)) % 60;
    const auto uptimeH = (m_uptime / (1000 * 60 * 60)) % 24;
    const auto uptimeD = (m_uptime / (1000 * 60 * 60 * 24));

    // TODO use snprintf
    char buff[12];
    snprintf(buff, sizeof(buff), "%02lld:%02lld:%02lld:%02lld",
             uptimeD.count(), uptimeH.count(), uptimeM.count(), uptimeS.count());

    return std::string{ buff };
}

float CPUMeasure::calculateCPULoad(uint64_t idleTicks, uint64_t totalTicks) {
    /* Credit to Jeremy Friesner
       http://stackoverflow.com/questions/23143693/retrieving-cpu-load-percent-total-in-windows-with-c */
    static uint64_t prevTotalTicks{ 0U };
    static uint64_t prevIdleTicks{ 0U };

    const uint64_t totalTicksSinceLastTime{ totalTicks - prevTotalTicks };
    const uint64_t idleTicksSinceLastTime{ idleTicks - prevIdleTicks };

    const float cpuLoad{ 1.0f - ((totalTicksSinceLastTime > 0) ?
                     (static_cast<float>(idleTicksSinceLastTime)) / totalTicksSinceLastTime : 0) };

    prevTotalTicks = totalTicks;
    prevIdleTicks = idleTicks;

    return cpuLoad;
}

}

