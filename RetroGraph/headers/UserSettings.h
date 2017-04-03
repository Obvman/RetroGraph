#pragma once

#include <stdint.h>
#include <string>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>

namespace rg {

namespace po = boost::program_options;
namespace pt = boost::property_tree;

class UserSettings {
public:
    UserSettings();
    ~UserSettings() noexcept = default;
    UserSettings(const UserSettings&) = delete;
    UserSettings& operator=(const UserSettings&) = delete;

    int32_t getStartupMonitor() const { return m_startupMonitor; }
    bool getClickthrough() const { return m_clickthrough; }
    bool getWidgetBackground() const { return m_widgetBackground; }

    const std::string& getPingServer() const { return m_pingServer; }
    uint32_t getPingFreq() const { return m_pingFreq; }

    uint32_t getNumProcessesDisplayed() const {
        return m_numProcessesDisplayed;
    }
    float getHighCPUUsageThreshold() const {
        return m_processCPUUsageThreshold;
    }
    uint32_t getHighRAMUsageThresholdMB() const {
        return m_processRAMUsageThresholdMB;
    }

    uint32_t getNetUsageSamples() const { return m_netUsageSamples; }
    uint32_t getCPUUsageSamples() const { return m_cpuUsageSamples; }
    uint32_t getGPUUsageSamples() const { return m_gpuUsageSamples; }
    uint32_t getRAMUsageSamples() const { return m_ramUsageSamples; }

private:
    void generateDefaultFile(pt::ptree& propTree);

    // Window options
    int32_t m_startupMonitor{ 0 };
    bool m_clickthrough{ true };
    bool m_widgetBackground{ false };

    // Network options
    std::string m_pingServer{ "http://www.google.com/" };
    uint32_t m_pingFreq{ 10U };

    // Appearance options
    uint32_t m_numProcessesDisplayed{ 8U };
    float m_processCPUUsageThreshold{ 0.2f };
    uint32_t m_processRAMUsageThresholdMB{ 1024U };

    // Widget options
    uint32_t m_netUsageSamples{ 20U };
    uint32_t m_cpuUsageSamples{ 20U };
    uint32_t m_gpuUsageSamples{ 20U };
    uint32_t m_ramUsageSamples{ 20U };
};

}
