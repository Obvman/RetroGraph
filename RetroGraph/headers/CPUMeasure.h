#pragma once

#include <GL/glew.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <chrono>

#include "CPUPlugin.h"

namespace rg {

class Window;

/* Measures statistics about the system CPU: Model name, total CPU load*/
class CPUMeasure {
public:
    CPUMeasure();
    ~CPUMeasure();

    void init();

    /* Updates the total system's CPU usage statistics */
    void update();

    /* Returns the current system CPU load as a percentage */
    float getCPULoad();

    /* Creates new string containing the current system uptime in
       dd:hh:mm:ss format */
    std::string getUptimeStr() const;

    const std::string& getCPUName() const { return m_cpuName; }

    const std::vector<float>& getUsageData() const { return m_usageData; }

    /* Returns the maximum number of CPU usage samples stored */
    size_t getDataSize() const { return dataSize; }

private:
    /* Calculates the total CPU load with the given tick information */
    float calculateCPULoad(uint64_t idleTicks, uint64_t totalTicks);

    CPUPlugin m_coreTempPlugin;
    size_t dataSize; // max number of usage percentages to store
    std::vector<float> m_usageData;
    std::chrono::milliseconds m_uptime;

    std::string m_cpuName;
};

}