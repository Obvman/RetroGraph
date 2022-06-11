module;

#include "RGAssert.h"

export module CPUMeasure;

import Colors;
import CPUPlugin;
import Measure;
import Units;
import UserSettings;
import Utils;

import <algorithm>;
import <chrono>;
import <ctime>;
import <vector>;

import <WindowsHeaders.h>;

namespace rg {

export class CPUMeasure;

/* Measures statistics about the system CPU: Model name, total CPU load*/
class CPUMeasure : public Measure {
public:
    CPUMeasure();
    ~CPUMeasure() noexcept = default;

    /* Updates the total system's CPU usage statistics */
    void update(int ticks) override;

    void refreshSettings() override;

    /* Returns the current system CPU load as a percentage */
    float getCPULoad();

    /* Creates new string containing the current system uptime in
       dd:hh:mm:ss format */
    std::string getUptimeStr() const;

    /* Returns the number of physical cores in the CPU */
    int getNumCores() const { return m_coreTempPlugin.getNumCores(); }

    /* Returns the current CPU clock speed in Megahertz */
    float getClockSpeed() const { return m_coreTempPlugin.getClockSpeed(); }

    /* Returns the current CPU voltage in volts */
    float getVoltage() const { return m_coreTempPlugin.getVoltage(); }

    /* Returns the temperature of the specified core */
    float getTemp(int coreNum) const {
        return m_coreTempPlugin.getTemp(coreNum);
    }

    /* Returns the maximum allowable CPU temperature in degrees celsius */
    int getTjMax() const { return m_coreTempPlugin.getTjMax(); }

    /* Gets description of the CPU model */
    const std::string& getCPUName() const { return m_cpuName; }

    /* Returns the CPU's total load history */
    const std::vector<float>& getUsageData() const { return m_usageData; }

    /* Returns list of each core's load history */
    const std::vector<std::vector<float>>& getPerCoreUsageData() const {
        return m_perCoreData;
    }

    /* Returns the maximum number of CPU usage samples stored */
    size_t getDataSize() const { return dataSize; }

    bool getCoreTempInfoSuccess() const {
        return m_coreTempPlugin.getCoreTempInfoSuccess();
    }

private:
    /* Fill CPU name if CoreTemp interfacing was successful */
    void updateCPUName();

    /* Calculates the total CPU load with the given tick information */
    float calculateCPULoad(uint64_t idleTicks, uint64_t totalTicks);

    /* Resets core usage data vector */
    void resetData();

    CPUPlugin m_coreTempPlugin{};

    size_t dataSize{ 40U };
    std::vector<float> m_usageData{};

    size_t perCoreDataSize{ 80U };
    std::vector<std::vector<float>> m_perCoreData{};

    std::chrono::milliseconds m_uptime{ 0 };
    std::string m_cpuName{ "" };
};

unsigned long long FileTimeToInt64(const FILETIME & ft) {
    return (((unsigned long long)(ft.dwHighDateTime)) << 32) 
        | ((unsigned long long)ft.dwLowDateTime);
}

CPUMeasure::CPUMeasure()
    : dataSize{ UserSettings::inst().getVal<int, size_t>("Widgets-CPUGraph.NumUsageSamples") }
    , m_usageData( dataSize, 0.0f ) {

    updateCPUName();

    // Create one vector for each core in the machine, and fill each vector with
    // default core usage values
    m_perCoreData.resize(m_coreTempPlugin.getNumCores());
    for (auto& vec : m_perCoreData) {
        vec.assign(perCoreDataSize, 0.0f);
    }
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
    std::rotate(m_usageData.begin(), m_usageData.begin() + 1,
                m_usageData.end());

    for (unsigned int i = 0U; i < m_perCoreData.size(); ++i) {
        const auto coreUsage = float{ m_coreTempPlugin.getLoad(i) / 100.0f };
        m_perCoreData[i][0] = coreUsage;
        std::rotate(m_perCoreData[i].begin(),
                    m_perCoreData[i].begin() + 1, m_perCoreData[i].end());
    }
}

void CPUMeasure::refreshSettings() {
    const size_t newDataSize{ UserSettings::inst().getVal<int, size_t>("Widgets-CPUGraph.NumUsageSamples") };
    if (dataSize == newDataSize)
        return;

    m_usageData.assign(newDataSize, 0.0f);
    dataSize = newDataSize;
}

void CPUMeasure::updateCPUName() {
    if (m_cpuName.empty() && m_coreTempPlugin.getCoreTempInfoSuccess())
        m_cpuName = "CPU: " + std::string{ m_coreTempPlugin.getCPUName() };
}

float CPUMeasure::getCPULoad() {
    FILETIME idleTime;
    FILETIME kernelTime;
    FILETIME userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return -1.0f;
    }

    return calculateCPULoad(FileTimeToInt64(idleTime),
                            FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime));
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

}

