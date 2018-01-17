#pragma once

#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>

#include "widgets.h"

namespace rg {

using settingVariant = std::variant<int32_t, uint32_t, bool, float, std::string>;

class UserSettings {
public:
    static UserSettings& inst() {
        static UserSettings i;
        return i;
    }
    ~UserSettings() noexcept = default;
    UserSettings(const UserSettings&) = delete;
    UserSettings& operator=(const UserSettings&) = delete;

    settingVariant getSettingValue(const std::string& settingName) const;

    bool isVisible(size_t w) const { return m_widgetVisibilities[w]; }
    WidgetPosition getWidgetPosition(size_t w) const { return m_widgetPositions[w]; }

    void toggleWidgetBackgroundVisible() { m_settings["Window.WidgetBackground"] = 
        !std::get<bool>(m_settings["Window.WidgetBackground"]); }

private:
    UserSettings();

    void generateDefaultFile(boost::property_tree::ptree& propTree);

    std::map<std::string, settingVariant> m_settings;

    std::vector<bool> m_widgetVisibilities;
    std::vector<WidgetPosition> m_widgetPositions;

    // TODO remove all these members and rewrite generateDefaultFile()
    // Window options
    bool m_clickthrough{ true };
    bool m_widgetBackground{ false };

    // Network options
    std::string m_pingServer{ "http://www.google.com/" };
    uint32_t m_pingFreq{ 10U };

    // Appearance options
    uint32_t m_numCPUProcessesDisplayed{ 8U };
    uint32_t m_numRAMProcessesDisplayed{ 8U };
    float m_processCPUUsageThreshold{ 0.2f };
    uint32_t m_processRAMUsageThresholdMB{ 1024U };

    // Widget options
    uint32_t m_netUsageSamples{ 20U };
    uint32_t m_cpuUsageSamples{ 20U };
    uint32_t m_gpuUsageSamples{ 20U };
    uint32_t m_ramUsageSamples{ 20U };

};

}
