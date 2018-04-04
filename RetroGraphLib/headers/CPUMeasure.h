#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <cstdint>

#include "CPUPlugin.h"

namespace rg {

class Window;

class UserSettings;

/* Measures statistics about the system CPU: Model name, total CPU load*/
class CPUMeasure {
public:
    CPUMeasure();
    ~CPUMeasure() noexcept = default;
    CPUMeasure(const CPUMeasure&) = delete;
    CPUMeasure& operator=(const CPUMeasure&) = delete;
    CPUMeasure(CPUMeasure&&) = delete;
    CPUMeasure& operator=(CPUMeasure&&) = delete;

    /* Updates the total system's CPU usage statistics */
    void update(uint32_t ticks);

    /* Returns the current system CPU load as a percentage */
    float getCPULoad();

    /* Creates new string containing the current system uptime in
       dd:hh:mm:ss format */
    std::string getUptimeStr() const;

    /* Returns the number of physical cores in the CPU */
    uint32_t getNumCores() const { return m_coreTempPlugin.getNumCores(); }

    /* Returns the current CPU clock speed in Megahertz */
    float getClockSpeed() const { return m_coreTempPlugin.getClockSpeed(); }

    /* Returns the current CPU voltage in volts */
    float getVoltage() const { return m_coreTempPlugin.getVoltage(); }

    /* Returns the temperature of the specified core */
    float getTemp(uint32_t coreNum) const {
        return m_coreTempPlugin.getTemp(coreNum);
    }

    /* Returns the maximum allowable CPU temperature in degrees celsius */
    uint32_t getTjMax() const { return m_coreTempPlugin.getTjMax(); }

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
    /* Calculates the total CPU load with the given tick information */
    float calculateCPULoad(uint64_t idleTicks, uint64_t totalTicks);

    /* Resets core usage data vector */
    void resetData();

    CPUPlugin m_coreTempPlugin{};

    const size_t dataSize{ 40U };
    std::vector<float> m_usageData{};

    const size_t perCoreDataSize{ 20U };
    std::vector<std::vector<float>> m_perCoreData{};

    std::chrono::milliseconds m_uptime{ 0 };
    std::string m_cpuName{ "" };
};

} // namespace rg
