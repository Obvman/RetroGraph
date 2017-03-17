#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <stdint.h>
#include <GL/glew.h>

#include "Measure.h"
#include "CPUPlugin.h"
#include "GraphData.h"

namespace rg {

class Window;

/* Measures statistics about the system CPU: Model name, total CPU load*/
class CPUMeasure : public Measure {
public:
    CPUMeasure();
    virtual ~CPUMeasure();
    CPUMeasure(const CPUMeasure&) = delete;
    CPUMeasure& operator=(const CPUMeasure&) = delete;

    virtual void init() override;

    /* Updates the total system's CPU usage statistics */
    virtual void update(uint32_t ticks) override;

    /* Returns the current system CPU load as a percentage */
    float getCPULoad();

    /* Creates new string containing the current system uptime in
       dd:hh:mm:ss format */
    std::string getUptimeStr() const;

    const std::string& getCPUName() const { return m_cpuName; }

    const std::vector<float>& getUsageData() const { return m_usageData; }

    const std::vector<std::vector<float>>& getPerCoreUsageData() const {
        return m_perCoreData;
    }

    /* Returns the maximum number of CPU usage samples stored */
    size_t getDataSize() const { return dataSize; }

private:
    /* Calculates the total CPU load with the given tick information */
    float calculateCPULoad(uint64_t idleTicks, uint64_t totalTicks);

    CPUPlugin m_coreTempPlugin;

    const size_t dataSize; // max number of usage percentages to store
    std::vector<float> m_usageData;

    const size_t perCoreDataSize;
    std::vector<std::vector<float>> m_perCoreData;

    std::chrono::milliseconds m_uptime;
    std::string m_cpuName;
};

}