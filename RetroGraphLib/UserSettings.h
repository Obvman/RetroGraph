#pragma once

#include "stdafx.h"

#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "Widget.h"
#include "utils.h"

class INIReader;

namespace rg {

using SettingVariant = std::variant<int, bool, double, std::string>;

class UserSettings {
public:
    static UserSettings& inst() { static UserSettings i; return i; }

    ~UserSettings() noexcept = default;
    UserSettings(const UserSettings&)            = delete;
    UserSettings& operator=(const UserSettings&) = delete;
    UserSettings(UserSettings&&)                 = delete;
    UserSettings& operator=(UserSettings&&)      = delete;

    template<typename T, typename CastT = T>
    auto getVal(std::string_view settingName) {
        if (m_settings.count(settingName) == 0)
            fatalMessageBox("Failed to find setting " + std::string{ settingName });

        // If CastT specified, do a static cast
        if constexpr (!std::is_same_v<T, CastT>) {
            return static_cast<CastT>(std::get<T>(m_settings.at(settingName)));
        } else {
            return std::get<T>(m_settings.at(settingName));
        }
    }

    bool isVisible(Widgets w) const { return m_widgetVisibilities[w]; }
    WidgetPosition getWidgetPosition(Widgets w) const { return m_widgetPositions[w]; }

    void toggleWidgetBackgroundVisible() { m_settings["Window.WidgetBackground"] = 
        !std::get<bool>(m_settings["Window.WidgetBackground"]); }

    void readConfig();

    // Write to the settings storage file.
    // Should be done after reading an ini file and on program shutdown.
    void writeDataFile() const;

    static const std::string iniPath;
    static const std::string fallbackIniPath;

private:
    UserSettings();

    // Initialises member values from ini reader
    void readMembers(const INIReader& reader);

    std::map<std::string_view, SettingVariant> m_settings;

    std::vector<bool> m_widgetVisibilities;
    std::vector<WidgetPosition> m_widgetPositions;

    const std::map<std::string_view, WidgetPosition> m_posMap = {
        {"top-left",      WidgetPosition::TOP_LEFT},
        {"top-middle",    WidgetPosition::TOP_MID},
        {"top-right",     WidgetPosition::TOP_RIGHT},
        {"middle-left",   WidgetPosition::MID_LEFT},
        {"middle-middle", WidgetPosition::MID_MID},
        {"middle-right",  WidgetPosition::MID_RIGHT},
        {"bottom-left",   WidgetPosition::BOT_LEFT},
        {"bottom-middle", WidgetPosition::BOT_MID},
        {"bottom-right",  WidgetPosition::BOT_RIGHT},
    };
};

} // namespace rg
