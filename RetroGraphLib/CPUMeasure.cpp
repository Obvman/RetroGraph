#include "stdafx.h"

#include "CPUMeasure.h"

#include <algorithm>
#include <ctime>
#include <Windows.h>
// #include <GL/glew.h>
// #include <GL/gl.h>
// #include <GL/freeglut.h>

#include "UserSettings.h"
#include "colors.h"
#include "units.h"
#include "utils.h"

namespace rg {

unsigned long long FileTimeToInt64(const FILETIME & ft) {
    return (((unsigned long long)(ft.dwHighDateTime)) << 32) 
        | ((unsigned long long)ft.dwLowDateTime);
}

CPUMeasure::CPUMeasure() :
    dataSize{ std::get<uint32_t>(
                  UserSettings::inst().getVal("Widgets-Graphs-CPU.NumUsageSamples")
              ) } {

    // Fill CPU name if CoreTemp interfacing was successful
    if (m_cpuName.size() == 0 && m_coreTempPlugin.getCoreTempInfoSuccess()) {
        m_cpuName = "CPU: ";
        m_cpuName.append(m_coreTempPlugin.getCPUName());
    }

    // fill vector with default values
    m_usageData.assign(dataSize, 0.0f);

    // Create one vector for each core in the machine, and fill each vector with
    // default core usage values
    m_perCoreData.resize(m_coreTempPlugin.getNumCores());
    for (auto& vec : m_perCoreData) {
        vec.assign(perCoreDataSize, 0.0f);
    }
}

void CPUMeasure::update(uint32_t ticks) {
    if (ticks % (ticksPerSecond / 2) == 0) {
        m_coreTempPlugin.update();

        /* If the coretemp program was started in the last frame, reset usage
         * vectors and resize them to the number of cores provided by coretemp
         */
        if (m_coreTempPlugin.coreTempWasStarted())
            resetData();

        m_uptime = std::chrono::milliseconds(GetTickCount64());

        // Fill CPU name if CoreTemp interfacing was successful 
        // TODO code duplication
        if (m_cpuName.size() == 0 && m_coreTempPlugin.getCoreTempInfoSuccess()) {
            m_cpuName = "CPU: ";
            m_cpuName.append(m_coreTempPlugin.getCPUName());
        }

        const auto totalLoad{ getCPULoad() };
        // Add to the usageData vector by overwriting the oldest value and
        // shifting the elements in the vector
        m_usageData[0] = totalLoad;
        std::rotate(m_usageData.begin(), m_usageData.begin() + 1,
                    m_usageData.end());

        for (auto i = size_t{ 0U }; i < m_perCoreData.size(); ++i) {
            const auto coreUsage = float{ m_coreTempPlugin.getLoad(i) / 100.0f };
            m_perCoreData[i][0] = coreUsage;
            std::rotate(m_perCoreData[i].begin(),
                        m_perCoreData[i].begin() + 1, m_perCoreData[i].end());
        }
    }
}

float CPUMeasure::getCPULoad() {
    FILETIME idleTime;
    FILETIME kernelTime;
    FILETIME userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return -1.0f;
    }

    return calculateCPULoad(FileTimeToInt64(idleTime),
                            FileTimeToInt64(kernelTime) +
                                FileTimeToInt64(userTime));
}

std::string CPUMeasure::getUptimeStr() const {
    const auto uptimeS{ (m_uptime / 1000) % 60 };
    const auto uptimeM{ (m_uptime / (60 * 1000)) % 60 };
    const auto uptimeH{ (m_uptime / (1000 * 60 * 60)) % 24 };
    const auto uptimeD{ (m_uptime / (1000 * 60 * 60 * 24)) };

    char buff[12];
    if (snprintf(buff, sizeof(buff), "%02lld:%02lld:%02lld:%02lld",
                 uptimeD.count(), uptimeH.count(), uptimeM.count(),
                 uptimeS.count()) < 0) {
        fatalMessageBox("snprintf() failed to copy uptime string");
    }

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
                         (static_cast<float>(idleTicksSinceLastTime)) /
                         totalTicksSinceLastTime : 0) };

    prevTotalTicks = totalTicks;
    prevIdleTicks = idleTicks;

    return cpuLoad;
}

void CPUMeasure::resetData() {
    m_perCoreData.clear();
    m_perCoreData.resize(m_coreTempPlugin.getNumCores());
    for (auto& vec : m_perCoreData) {
        vec.assign(perCoreDataSize, 0.0f);
    }
}

}

