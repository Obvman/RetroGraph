module UserSettings;

import std.filesystem;

import "RGAssert.h";

namespace rg {

UserSettings::UserSettings()
    : m_iniPath{ getExePath() + R"(\..\..\RetroGraph\Resources\config.ini)" }
    , m_fallbackIniPath{ getExePath() + R"(\..\..\RetroGraph\Resources\default_config.ini)" }
    , m_widgetVisibilities(static_cast<int>(WidgetType::NumWidgets), true)
    , m_widgetPositions(static_cast<int>(WidgetType::NumWidgets))
    , m_refreshProcs{}
    , m_refreshProcCounter{ 0 } {

    readConfig();
}

void UserSettings::toggleWidgetBackgroundVisible() {
    m_settings["Window.WidgetBackground"] = !std::get<bool>(m_settings["Window.WidgetBackground"]);
}
bool UserSettings::checkConfigChanged() const {
    static const auto autoReadConfig{ UserSettings::inst().getVal<bool>("Application.AutoReadConfig") };
    if (!autoReadConfig)
        return false;

    struct stat fileStat;
    if (stat(UserSettings::m_iniPath.c_str(), &fileStat) == 0) {
        auto newModTime{ fileStat.st_mtime };
        static auto modTime{ newModTime };

        bool result = newModTime != modTime;
        modTime = newModTime;
        return result;
    }

    return false;
}

void UserSettings::refresh() {
    readConfig();
    for (const auto& [handle, refreshProc] : m_refreshProcs)
        refreshProc();
}

void UserSettings::readConfig() {
    auto reader = INIReader{ m_iniPath };
    if (reader.ParseError() < 0) {
        showMessageBox(std::format("Failed to load config file from {}\nUsing default_config.ini", m_iniPath).c_str());
        reader = INIReader{ m_fallbackIniPath };

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

    m_widgetVisibilities[static_cast<int>(WidgetType::Time)]        = reader.GetBoolean("Widgets-Time",         "Visible", true);
    m_widgetVisibilities[static_cast<int>(WidgetType::CPUStats)]    = reader.GetBoolean("Widgets-CPUStats",     "Visible", true);
    m_widgetVisibilities[static_cast<int>(WidgetType::SystemStats)] = reader.GetBoolean("Widgets-SystemStats",  "Visible", true);
    m_widgetVisibilities[static_cast<int>(WidgetType::ProcessCPU)]  = reader.GetBoolean("Widgets-ProcessesCPU", "Visible", true);
    m_widgetVisibilities[static_cast<int>(WidgetType::ProcessRAM)]  = reader.GetBoolean("Widgets-ProcessesRAM", "Visible", true);
    m_widgetVisibilities[static_cast<int>(WidgetType::Music)]       = reader.GetBoolean("Widgets-Music",        "Visible", true);
    m_widgetVisibilities[static_cast<int>(WidgetType::Main)]        = reader.GetBoolean("Widgets-Main",         "Visible", true);
    m_widgetVisibilities[static_cast<int>(WidgetType::HDD)]         = reader.GetBoolean("Widgets-Drives",       "Visible", true);
    m_widgetVisibilities[static_cast<int>(WidgetType::FPS)]         = reader.GetBoolean("Widgets-FPS",          "Visible", true);
    m_widgetVisibilities[static_cast<int>(WidgetType::NetStats)]    = reader.GetBoolean("Widgets-NetStats",     "Visible", true);
    m_widgetVisibilities[static_cast<int>(WidgetType::CPUGraph)]    = reader.GetBoolean("Widgets-CPUGraph",     "Visible", true);
    m_widgetVisibilities[static_cast<int>(WidgetType::RAMGraph)]    = reader.GetBoolean("Widgets-RAMGraph",     "Visible", true);
    m_widgetVisibilities[static_cast<int>(WidgetType::NetGraph)]    = reader.GetBoolean("Widgets-NetGraph",     "Visible", true);
    m_widgetVisibilities[static_cast<int>(WidgetType::GPUGraph)]    = reader.GetBoolean("Widgets-GPUGraph",     "Visible", true);

    m_widgetPositions[static_cast<int>(WidgetType::ProcessCPU)]  = m_posMap.at(reader.Get("Widgets-ProcessesCPU", "Position", "top-middle"));
    m_widgetPositions[static_cast<int>(WidgetType::ProcessRAM)]  = m_posMap.at(reader.Get("Widgets-ProcessesRAM", "Position", "top-middle"));
    m_widgetPositions[static_cast<int>(WidgetType::Time)]        = m_posMap.at(reader.Get("Widgets-Time",         "Position", "top-left"));
    m_widgetPositions[static_cast<int>(WidgetType::SystemStats)] = m_posMap.at(reader.Get("Widgets-SystemStats",  "Position", "bottom-middle"));
    m_widgetPositions[static_cast<int>(WidgetType::Music)]       = m_posMap.at(reader.Get("Widgets-Music",        "Position", "bottom-right"));
    m_widgetPositions[static_cast<int>(WidgetType::CPUStats)]    = m_posMap.at(reader.Get("Widgets-CPUStats",     "Position", "middle-right"));
    m_widgetPositions[static_cast<int>(WidgetType::HDD)]         = m_posMap.at(reader.Get("Widgets-Drives",       "Position", "top-right"));
    m_widgetPositions[static_cast<int>(WidgetType::Main)]        = m_posMap.at(reader.Get("Widgets-Main",         "Position", "middle-middle"));
    m_widgetPositions[static_cast<int>(WidgetType::FPS)]         = m_posMap.at(reader.Get("Widgets-FPS",          "Position", "bottom-left"));
    m_widgetPositions[static_cast<int>(WidgetType::NetStats)]    = m_posMap.at(reader.Get("Widgets-NetStats",     "Position", "bottom-middle"));
    m_widgetPositions[static_cast<int>(WidgetType::CPUGraph)]    = m_posMap.at(reader.Get("Widgets-CPUGraph",     "Position", "middle-left"));
    m_widgetPositions[static_cast<int>(WidgetType::RAMGraph)]    = m_posMap.at(reader.Get("Widgets-RAMGraph",     "Position", "middle-left"));
    m_widgetPositions[static_cast<int>(WidgetType::NetGraph)]    = m_posMap.at(reader.Get("Widgets-NetGraph",     "Position", "middle-left"));
    m_widgetPositions[static_cast<int>(WidgetType::GPUGraph)]    = m_posMap.at(reader.Get("Widgets-GPUGraph",     "Position", "middle-left"));
}

RefreshProcHandle UserSettings::registerRefreshProc(std::function<void(void)> const& refreshProc) {
    m_refreshProcs.insert_or_assign(m_refreshProcCounter, refreshProc);
    return m_refreshProcCounter++;
}

void UserSettings::releaseRefreshProc(RefreshProcHandle handle) {
    m_refreshProcs.erase(handle);
}

} // namespace rg
