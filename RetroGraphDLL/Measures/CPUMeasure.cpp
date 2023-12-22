module RG.Measures:CPUMeasure;

import "RGAssert.h";

namespace rg {

unsigned long long fileTimeToInt64(const FILETIME& ft) {
    return ((static_cast<unsigned long long>(ft.dwHighDateTime)) << 32) |
           (static_cast<unsigned long long>(ft.dwLowDateTime));
}

CPUMeasure::CPUMeasure()
    : Measure{ seconds{ 1 } } {
    updateCPUName();
}

CPUMeasure::~CPUMeasure() {}

bool CPUMeasure::updateInternal() {
    m_coreTempPlugin.update();

    if (m_coreTempPlugin.coreTempWasStarted())
        onCPUCoreDataStateChanged.raise(true);
    if (m_coreTempPlugin.coreTempWasStopped())
        onCPUCoreDataStateChanged.raise(false);

    m_uptime = std::chrono::milliseconds(GetTickCount64());

    updateCPUName();

    if (getCoreTempInfoSuccess()) {
        for (int i{ 0 }; i < getNumCores(); ++i) {
            const auto coreUsage = float{ m_coreTempPlugin.getLoad(i) / 100.0f };
            onCPUCoreUsage.raise(i, coreUsage);
        }
    }

    onCPUUsage.raise(getCPULoad());
    return true;
}

void CPUMeasure::updateCPUName() {
    if (m_cpuName.empty() && getCoreTempInfoSuccess()) {
        const auto cpuName{ m_coreTempPlugin.getCPUName() };
        if (!cpuName.empty())
            m_cpuName = "CPU: " + cpuName;
    }
}

float CPUMeasure::getCPULoad() {
    FILETIME idleTime;
    FILETIME kernelTime;
    FILETIME userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return -1.0f;
    }

    return calculateCPULoad(fileTimeToInt64(idleTime), fileTimeToInt64(kernelTime) + fileTimeToInt64(userTime));
}

std::string CPUMeasure::getUptimeStr() const {
    const auto uptimeS{ (m_uptime / 1000) % 60 };
    const auto uptimeM{ (m_uptime / (60 * 1000)) % 60 };
    const auto uptimeH{ (m_uptime / (1000 * 60 * 60)) % 24 };
    const auto uptimeD{ (m_uptime / (1000 * 60 * 60 * 24)) };

    char buff[12];
    RGVERIFY(snprintf(buff, sizeof(buff), "%02lld:%02lld:%02lld:%02lld", uptimeD.count(), uptimeH.count(),
                      uptimeM.count(), uptimeS.count()) >= 0,
             "snprintf() failed to copy uptime string");

    return std::string{ buff };
}

float CPUMeasure::calculateCPULoad(uint64_t idleTicks, uint64_t totalTicks) {
    /* Credit to Jeremy Friesner
       http://stackoverflow.com/questions/23143693/retrieving-cpu-load-percent-total-in-windows-with-c */
    static uint64_t prevTotalTicks{ 0U };
    static uint64_t prevIdleTicks{ 0U };

    const uint64_t totalTicksSinceLastTime{ totalTicks - prevTotalTicks };
    const uint64_t idleTicksSinceLastTime{ idleTicks - prevIdleTicks };

    const float cpuLoad{ 1.0f - ((totalTicksSinceLastTime > 0)
                                     ? (static_cast<float>(idleTicksSinceLastTime)) / totalTicksSinceLastTime
                                     : 0) };

    prevTotalTicks = totalTicks;
    prevIdleTicks = idleTicks;

    return cpuLoad;
}

} // namespace rg
