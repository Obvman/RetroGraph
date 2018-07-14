#pragma once

#include "stdafx.h"

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <variant>

#include "Widget.h"
#include "utils.h"

class INIReader;

namespace rg {

using settingVariant = std::variant<int, bool, double, std::string>;

class UserSettings {
public:
    static UserSettings& inst() { static UserSettings i; return i; }

    ~UserSettings() noexcept = default;
    UserSettings(const UserSettings&) = delete;
    UserSettings& operator=(const UserSettings&) = delete;
    UserSettings(UserSettings&&) = delete;
    UserSettings& operator=(UserSettings&&) = delete;

    template<typename T>
    T getVal(const std::string& settingName) const {
        if (m_settings.count(settingName) == 0) {
            fatalMessageBox("Failed to find setting " + settingName);
        }
        return std::get<T>(m_settings.at(settingName));
    }

    template<typename T, typename CastType>
    CastType getVal(const std::string& settingName) const {
        if (m_settings.count(settingName) == 0) {
            fatalMessageBox("Failed to find setting " + settingName);
        }
        return static_cast<CastType>(std::get<T>(m_settings.at(settingName)));
    }

    // String specialization - returns reference
    template<>
    const std::string& getVal<std::string>(const std::string& settingName) const {
        if (m_settings.count(settingName) == 0) {
            fatalMessageBox("Failed to find setting " + settingName);
        }
        return std::get<std::string>(m_settings.at(settingName));
    }

    bool isVisible(Widgets w) const { return m_widgetVisibilities[w]; }
    WidgetPosition getWidgetPosition(Widgets w) const { return m_widgetPositions[w]; }

    void toggleWidgetBackgroundVisible() { m_settings["Window.WidgetBackground"] = 
        !std::get<bool>(m_settings["Window.WidgetBackground"]); }

private:
    UserSettings();

    // Initialises member values from ini reader
    void readMembers(const INIReader& reader);

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

} // namespace rg
