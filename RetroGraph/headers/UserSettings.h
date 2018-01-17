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

    settingVariant getVal(const std::string& settingName) const;

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

    const std::map<std::string, WidgetPosition> m_posMap = {
        {"top-left", WidgetPosition::TOP_LEFT},
        {"top-middle", WidgetPosition::TOP_MID},
        {"top-right", WidgetPosition::TOP_RIGHT},
        {"middle-left", WidgetPosition::MID_LEFT},
        {"middle-middle", WidgetPosition::MID_MID},
        {"middle-right", WidgetPosition::MID_RIGHT},
        {"bottom-left", WidgetPosition::BOT_LEFT},
        {"bottom-middle", WidgetPosition::BOT_MID},
        {"bottom-right", WidgetPosition::BOT_RIGHT},
    };
};

}
