export module RG.UserSettings;

export import :ConfigRefreshedEvent;

import Utils;

import RG.Core;

import std;

import "RGAssert.h";
import "WindowsHeaderUnit.h";

import <inih/INIReader.h>;

namespace rg {

using SettingVariant = std::variant<int, bool, double, std::string>;

export class UserSettings {
public:
    static UserSettings& inst() {
        static UserSettings i;
        return i;
    }

    ~UserSettings() noexcept = default;
    UserSettings(const UserSettings&) = delete;
    UserSettings& operator=(const UserSettings&) = delete;
    UserSettings(UserSettings&&) = delete;
    UserSettings& operator=(UserSettings&&) = delete;

    template<typename T, typename CastT = T>
        requires std::convertible_to<T, CastT>
    auto getVal(std::string_view settingName) {
        RGASSERT(m_settings.count(settingName) != 0, std::format("Failed to find setting {}", settingName).c_str());

        // If CastT specified, do a static cast
        if constexpr (!std::is_same_v<T, CastT>) {
            return static_cast<CastT>(std::get<T>(m_settings.at(settingName)));
        } else {
            return std::get<T>(m_settings.at(settingName));
        }
    }

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
};

} // namespace rg
