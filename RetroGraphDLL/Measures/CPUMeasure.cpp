module Measures.CPUMeasure;

import "RGAssert.h";

namespace rg {

unsigned long long fileTimeToInt64(const FILETIME & ft) {
    return ((static_cast<unsigned long long> (ft.dwHighDateTime)) << 32) 
        | (static_cast<unsigned long long> (ft.dwLowDateTime));
}

CPUMeasure::CPUMeasure()
    : m_dataSize{ UserSettings::inst().getVal<int, size_t>("Widgets-CPUGraph.NumUsageSamples") }
    , m_usageData( m_dataSize, 0.0f )
    , m_refreshProcHandle{
        UserSettings::inst().registerRefreshProc(
            [&]() {
                const size_t newDataSize{ UserSettings::inst().getVal<int, size_t>("Widgets-CPUGraph.NumUsageSamples") };
                if (m_dataSize != newDataSize) {
                    m_usageData.assign(newDataSize, 0.0f);
                    m_dataSize = newDataSize;
                }
            })
    } {

    updateCPUName();

    // Create one vector for each core in the machine, and fill each vector with
    // default core usage values
    m_perCoreData.resize(m_coreTempPlugin.getNumCores());
    for (auto& vec : m_perCoreData) {
        vec.assign(perCoreDataSize, 0.0f);
    }
}

CPUMeasure::~CPUMeasure() {
    UserSettings::inst().releaseRefreshProc(m_refreshProcHandle);
}

void CPUMeasure::update(int) {
    m_coreTempPlugin.update();

    /* If the coretemp program was started in the last frame, reset usage
     * vectors and resize them to the number of cores provided by coretemp
     */
    if (m_coreTempPlugin.coreTempWasStarted())
        resetData();

    m_uptime = std::chrono::milliseconds(GetTickCount64());

    updateCPUName();

    const auto totalLoad{ getCPULoad() };
    // Add to the usageData vector by overwriting the oldest value and
    // shifting the elements in the vector
    m_usageData[0] = totalLoad;
    std::rotate(m_usageData.begin(), m_usageData.begin() + 1, m_usageData.end());

    for (unsigned int i = 0U; i < m_perCoreData.size(); ++i) {
        const auto coreUsage = float{ m_coreTempPlugin.getLoad(i) / 100.0f };
        m_perCoreData[i][0] = coreUsage;
        std::rotate(m_perCoreData[i].begin(), m_perCoreData[i].begin() + 1, m_perCoreData[i].end());
    }

    postUpdate();
}

void CPUMeasure::updateCPUName() {
    if (m_cpuName.empty() && m_coreTempPlugin.getCoreTempInfoSuccess()) {
        const auto cpuName{ m_coreTempPlugin.getCPUName() };
        if (!cpuName.empty())
            m_cpuName = "CPU: " + std::string{ cpuName };
    }
}

float CPUMeasure::getCPULoad() {
    FILETIME idleTime;
    FILETIME kernelTime;
    FILETIME userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return -1.0f;
    }

    return calculateCPULoad(fileTimeToInt64(idleTime),
                            fileTimeToInt64(kernelTime) + fileTimeToInt64(userTime));
}

std::string CPUMeasure::getUptimeStr() const {
    const auto uptimeS{ (m_uptime / 1000) % 60 };
    const auto uptimeM{ (m_uptime / (60 * 1000)) % 60 };
    const auto uptimeH{ (m_uptime / (1000 * 60 * 60)) % 24 };
    const auto uptimeD{ (m_uptime / (1000 * 60 * 60 * 24)) };

    char buff[12];
    RGVERIFY(snprintf(buff, sizeof(buff), "%02lld:%02lld:%02lld:%02lld",
                      uptimeD.count(), uptimeH.count(), uptimeM.count(), uptimeS.count()) >= 0
             , "snprintf() failed to copy uptime string");

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

} // namespace rg
