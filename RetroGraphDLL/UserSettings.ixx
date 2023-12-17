export module UserSettings;

import Utils;
import ConfigRefreshedEvent;

import RG.Core;
import RG.Widgets;

import std.core;

import "RGAssert.h";
import "WindowsHeaderUnit.h";

import <inih/INIReader.h>;

namespace rg {

using SettingVariant = std::variant<int, bool, double, std::string>;

export class UserSettings {
public:
    static UserSettings& inst() { static UserSettings i; return i; }

    ~UserSettings() noexcept = default;
    UserSettings(const UserSettings&)            = delete;
    UserSettings& operator=(const UserSettings&) = delete;
    UserSettings(UserSettings&&)                 = delete;
    UserSettings& operator=(UserSettings&&)      = delete;

    template<typename T, typename CastT = T>
    requires std::convertible_to<T, CastT>
    auto getVal(std::string_view settingName) {
        RGASSERT (m_settings.count(settingName) != 0, std::format("Failed to find setting {}", settingName).c_str());

        // If CastT specified, do a static cast
        if constexpr (!std::is_same_v<T, CastT>) {
            return static_cast<CastT>(std::get<T>(m_settings.at(settingName)));
        } else {
            return std::get<T>(m_settings.at(settingName));
        }
    }

    bool isVisible(WidgetType w) const { return m_widgetVisibilities[static_cast<int>(w)]; }
    WidgetPosition getWidgetPosition(WidgetType w) const { return m_widgetPositions[static_cast<int>(w)]; }

    void toggleWidgetBackgroundVisible();

    bool checkConfigChanged() const;
    void refresh();

    ConfigRefreshedEvent configRefreshed;

private:
    UserSettings();

    void readConfig();

    // Initialises member values from ini reader
    void readMembers(const INIReader& reader);

    std::string m_iniPath{ getExePath() + R"(\..\..\RetroGraph\Resources\config.ini)" };
    std::string m_fallbackIniPath{ getExePath() + R"(\..\..\RetroGraph\Resources\default_config.ini)" };

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
