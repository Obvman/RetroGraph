export module UserSettings;

import Utils;

import Widgets.WidgetDefines;

import std.core;
import std.filesystem;

import "RGAssert.h";
import "WindowsHeaderUnit.h";
import <inih/INIReader.h>;

namespace rg {

export class UserSettings;

export using SettingVariant = std::variant<int, bool, double, std::string>;

class UserSettings {
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

    bool isVisible(Widgets w) const { return m_widgetVisibilities[static_cast<int>(w)]; }
    WidgetPosition getWidgetPosition(Widgets w) const { return m_widgetPositions[static_cast<int>(w)]; }

    void toggleWidgetBackgroundVisible() { m_settings["Window.WidgetBackground"] = 
        !std::get<bool>(m_settings["Window.WidgetBackground"]); }

    void readConfig();

    // Write to the settings storage file.
    // Should be done after reading an ini file and on program shutdown.
    void writeDataFile() const;

    static inline const std::string iniPath{ getExePath() + R"(\..\..\RetroGraph\Resources\config.ini)" };
    static inline const std::string fallbackIniPath{ getExePath() + R"(\..\..\RetroGraph\Resources\default_config.ini)" };

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

UserSettings::UserSettings()
    : m_widgetVisibilities(static_cast<int>(Widgets::NumWidgets), true)
    , m_widgetPositions(static_cast<int>(Widgets::NumWidgets)) {

    // TODO only read config if no settings file
    readConfig();

    writeDataFile();
}

void UserSettings::readConfig() {
    auto reader = INIReader{ iniPath };
    if (reader.ParseError() < 0) {
        showMessageBox(std::format("Failed to load config file from {}\nUsing default_config.ini", iniPath).c_str());
        reader = INIReader{ fallbackIniPath };

        RGASSERT(reader.ParseError() >= 0, "Failed to load default config file.");
    }

    try {
        readMembers(reader);
    } catch (const std::exception& e) {
        RGERROR(std::format("Config file path error: {}", e.what()).c_str());
    }
}

void UserSettings::readMembers(const INIReader& reader) {
    m_settings["Application.AutoReadConfig"]                   = reader.GetBoolean("Application",          "AutoReadConfig", true);
    m_settings["Window.Monitor"]                               = reader.GetInteger("Window",               "Monitor", 0);
    m_settings["Window.WidgetBackground"]                      = reader.GetBoolean("Window",               "WidgetBackground", true);
    m_settings["Network.PingServer"]                           = reader.Get       ("Network",              "PingServer", "http://www.google.com/");
    m_settings["Network.PingFrequency"]                        = reader.GetInteger("Network",              "PingFrequency", 10);
    m_settings["Widgets-ProcessesCPU.NumProcessesDisplayed"]   = reader.GetInteger("Widgets-ProcessesCPU", "NumProcessesDisplayed", 10);
    m_settings["Widgets-ProcessesCPU.HighCPUUsageThreshold"]   = reader.GetReal   ("Widgets-ProcessesCPU", "HighCPUUsageThreshold", 0.2);
    m_settings["Widgets-ProcessesRAM.NumProcessesDisplayed"]   = reader.GetInteger("Widgets-ProcessesRAM", "NumProcessesDisplayed", 10);
    m_settings["Widgets-ProcessesRAM.HighRAMUsageThresholdMB"] = reader.GetInteger("Widgets-ProcessesRAM", "HighRAMUsageThresholdMB", 1024);
    m_settings["Widgets-NetGraph.NumUsageSamples"]             = reader.GetInteger("Widgets-NetGraph",     "NumUsageSamples", 40);
    m_settings["Widgets-CPUGraph.NumUsageSamples"]             = reader.GetInteger("Widgets-CPUGraph",     "NumUsageSamples", 40);
    m_settings["Widgets-GPUGraph.NumUsageSamples"]             = reader.GetInteger("Widgets-GPUGraph",     "NumUsageSamples", 40);
    m_settings["Widgets-RAMGraph.NumUsageSamples"]             = reader.GetInteger("Widgets-RAMGraph",     "NumUsageSamples", 40);
    m_settings["Widgets-Main.FPS"]                             = reader.GetInteger("Widgets-Main",         "FPS", 30);

    m_widgetVisibilities[static_cast<int>(Widgets::Time)]        = reader.GetBoolean("Widgets-Time",         "Visible", true);
    m_widgetVisibilities[static_cast<int>(Widgets::CPUStats)]    = reader.GetBoolean("Widgets-CPUStats",     "Visible", true);
    m_widgetVisibilities[static_cast<int>(Widgets::SystemStats)] = reader.GetBoolean("Widgets-SystemStats",  "Visible", true);
    m_widgetVisibilities[static_cast<int>(Widgets::ProcessCPU)]  = reader.GetBoolean("Widgets-ProcessesCPU", "Visible", true);
    m_widgetVisibilities[static_cast<int>(Widgets::ProcessRAM)]  = reader.GetBoolean("Widgets-ProcessesRAM", "Visible", true);
    m_widgetVisibilities[static_cast<int>(Widgets::Music)]       = reader.GetBoolean("Widgets-Music",        "Visible", true);
    m_widgetVisibilities[static_cast<int>(Widgets::Main)]        = reader.GetBoolean("Widgets-Main",         "Visible", true);
    m_widgetVisibilities[static_cast<int>(Widgets::HDD)]         = reader.GetBoolean("Widgets-Drives",       "Visible", true);
    m_widgetVisibilities[static_cast<int>(Widgets::FPS)]         = reader.GetBoolean("Widgets-FPS",          "Visible", true);
    m_widgetVisibilities[static_cast<int>(Widgets::NetStats)]    = reader.GetBoolean("Widgets-NetStats",     "Visible", true);
    m_widgetVisibilities[static_cast<int>(Widgets::CPUGraph)]    = reader.GetBoolean("Widgets-CPUGraph",     "Visible", true);
    m_widgetVisibilities[static_cast<int>(Widgets::RAMGraph)]    = reader.GetBoolean("Widgets-RAMGraph",     "Visible", true);
    m_widgetVisibilities[static_cast<int>(Widgets::NetGraph)]    = reader.GetBoolean("Widgets-NetGraph",     "Visible", true);
    m_widgetVisibilities[static_cast<int>(Widgets::GPUGraph)]    = reader.GetBoolean("Widgets-GPUGraph",     "Visible", true);

    m_widgetPositions[static_cast<int>(Widgets::ProcessCPU)]  = m_posMap.at(reader.Get("Widgets-ProcessesCPU", "Position", "top-middle"));
    m_widgetPositions[static_cast<int>(Widgets::ProcessRAM)]  = m_posMap.at(reader.Get("Widgets-ProcessesRAM", "Position", "top-middle"));
    m_widgetPositions[static_cast<int>(Widgets::Time)]        = m_posMap.at(reader.Get("Widgets-Time",         "Position", "top-left"));
    m_widgetPositions[static_cast<int>(Widgets::SystemStats)] = m_posMap.at(reader.Get("Widgets-SystemStats",  "Position", "bottom-middle"));
    m_widgetPositions[static_cast<int>(Widgets::Music)]       = m_posMap.at(reader.Get("Widgets-Music",        "Position", "bottom-right"));
    m_widgetPositions[static_cast<int>(Widgets::CPUStats)]    = m_posMap.at(reader.Get("Widgets-CPUStats",     "Position", "middle-right"));
    m_widgetPositions[static_cast<int>(Widgets::HDD)]         = m_posMap.at(reader.Get("Widgets-Drives",       "Position", "top-right"));
    m_widgetPositions[static_cast<int>(Widgets::Main)]        = m_posMap.at(reader.Get("Widgets-Main",         "Position", "middle-middle"));
    m_widgetPositions[static_cast<int>(Widgets::FPS)]         = m_posMap.at(reader.Get("Widgets-FPS",          "Position", "bottom-left"));
    m_widgetPositions[static_cast<int>(Widgets::NetStats)]    = m_posMap.at(reader.Get("Widgets-NetStats",     "Position", "bottom-middle"));
    m_widgetPositions[static_cast<int>(Widgets::CPUGraph)]    = m_posMap.at(reader.Get("Widgets-CPUGraph",     "Position", "middle-left"));
    m_widgetPositions[static_cast<int>(Widgets::RAMGraph)]    = m_posMap.at(reader.Get("Widgets-RAMGraph",     "Position", "middle-left"));
    m_widgetPositions[static_cast<int>(Widgets::NetGraph)]    = m_posMap.at(reader.Get("Widgets-NetGraph",     "Position", "middle-left"));
    m_widgetPositions[static_cast<int>(Widgets::GPUGraph)]    = m_posMap.at(reader.Get("Widgets-GPUGraph",     "Position", "middle-left"));
}

void UserSettings::writeDataFile() const {
    using namespace std::filesystem;

    const auto configFilePath = []() {
        if constexpr (debugMode) {
            return path{ getExePath() + R"(\..\..\DEBUGSETTINGS\settingsDEBUG)" };
        } else {
            return path{ getExpandedEnvPath(R"(%APPDATA%\RetroGraph\settings)") };
        }
    }(); // Call immediately

    const auto configFileDir{ configFilePath.parent_path() };
    if (!exists(configFileDir)) {
        create_directories(configFileDir);
    }

    std::ofstream outFile{ configFilePath };
    RGASSERT(!!outFile, "Couldn't write settings to file");

    // Write all the options to file
    for (const auto& [key, value] : m_settings) {
        std::visit([&outFile, &key](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                outFile << key << ' ' << arg << '\n';
            } else if constexpr (std::is_same_v<T, bool>) {
                outFile << key << std::boolalpha << ' ' << arg << '\n';
            } else if constexpr (std::is_same_v<T, double>) {
                outFile << key << ' ' << arg << '\n';
            } else if constexpr (std::is_same_v<T, std::string>) {
                outFile << key << ' ' << arg << '\n';
            }
        }, value);
    }

    for (auto i = int{ 0 }; i < m_widgetVisibilities.size(); ++i) {
        outFile << i << ' ' << m_widgetVisibilities[i] << '\n';
    }

    for (auto i = int{ 0 }; i < m_widgetPositions.size(); ++i) {
        outFile << i << ' ' << static_cast<int>(m_widgetPositions[i]) << '\n';
    }
}

}
