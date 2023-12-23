module RG.UserSettings;

import std.filesystem;

import "RGAssert.h";

namespace rg {

UserSettings::UserSettings()
    : m_iniPath{ getExePath() + R"(\..\..\RetroGraph\Resources\config.ini)" }
    , m_fallbackIniPath{ getExePath() + R"(\..\..\RetroGraph\Resources\default_config.ini)" } {
    readConfig();
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
    configRefreshed.raise();
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
    // #TODO typesafe keys
    m_settings["Application.AutoReadConfig"] = reader.GetBoolean("Application", "AutoReadConfig", true);
    m_settings["Application.FPS"] = reader.GetInteger("Application", "FPS", 30);
    m_settings["Window.Monitor"] = reader.GetInteger("Window", "Monitor", 0);
    m_settings["Window.WidgetBackground"] = reader.GetBoolean("Window", "WidgetBackground", true);

    m_settings["Measures-CPU.UpdateInterval"] = reader.GetInteger("Measures-CPU", "UpdateInterval", 1000);
    m_settings["Measures-Drive.UpdateInterval"] = reader.GetInteger("Measures-Drive", "UpdateInterval", 30000);
    m_settings["Measures-Music.UpdateInterval"] = reader.GetInteger("Measures-Music", "UpdateInterval", 1000);
    m_settings["Measures-Net.PingServer"] = reader.Get("Measures-Net", "PingServer", "http://www.google.com/");
    m_settings["Measures-Net.PingFrequency"] = reader.GetInteger("Measures-Net", "PingFrequency", 10);
    m_settings["Measures-RAM.UpdateInterval"] = reader.GetInteger("Measures-RAM", "UpdateInterval", 1000);
    m_settings["Measures-Time.UpdateInterval"] = reader.GetInteger("Measures-Time", "UpdateInterval", 1000);

    m_settings["Widgets-ProcessesCPU.NumProcessesDisplayed"] =
        reader.GetInteger("Widgets-ProcessesCPU", "NumProcessesDisplayed", 10);
    m_settings["Widgets-ProcessesCPU.HighCPUUsageThreshold"] =
        reader.GetReal("Widgets-ProcessesCPU", "HighCPUUsageThreshold", 0.2);
    m_settings["Widgets-ProcessesRAM.NumProcessesDisplayed"] =
        reader.GetInteger("Widgets-ProcessesRAM", "NumProcessesDisplayed", 10);
    m_settings["Widgets-ProcessesRAM.HighRAMUsageThresholdMB"] =
        reader.GetInteger("Widgets-ProcessesRAM", "HighRAMUsageThresholdMB", 1024);
    m_settings["Widgets-NetGraph.NumUsageSamples"] = reader.GetInteger("Widgets-NetGraph", "NumUsageSamples", 40);
    m_settings["Widgets-NetGraph.DownloadDataScaleLowerBoundKB"] =
        reader.GetInteger("Widgets-NetGraph", "DownloadDataScaleLowerBoundKB", 100);
    m_settings["Widgets-NetGraph.UploadDataScaleLowerBoundKB"] =
        reader.GetInteger("Widgets-NetGraph", "UploadDataScaleLowerBoundKB", 100);
    m_settings["Widgets-CPUGraph.NumUsageSamples"] = reader.GetInteger("Widgets-CPUGraph", "NumUsageSamples", 40);
    m_settings["Widgets-CPUStats.NumUsageSamples"] = reader.GetInteger("Widgets-CPUStats", "NumUsageSamples", 40);
    m_settings["Widgets-GPUGraph.NumUsageSamples"] = reader.GetInteger("Widgets-GPUGraph", "NumUsageSamples", 40);
    m_settings["Widgets-RAMGraph.NumUsageSamples"] = reader.GetInteger("Widgets-RAMGraph", "NumUsageSamples", 40);

    m_settings["Widgets-Time.Visible"] = reader.GetBoolean("Widgets-Time", "Visible", true);
    m_settings["Widgets-CPUStats.Visible"] = reader.GetBoolean("Widgets-CPUStats", "Visible", true);
    m_settings["Widgets-SystemStats.Visible"] = reader.GetBoolean("Widgets-SystemStats", "Visible", true);
    m_settings["Widgets-ProcessesCPU.Visible"] = reader.GetBoolean("Widgets-ProcessesCPU", "Visible", true);
    m_settings["Widgets-ProcessesRAM.Visible"] = reader.GetBoolean("Widgets-ProcessesRAM", "Visible", true);
    m_settings["Widgets-Music.Visible"] = reader.GetBoolean("Widgets-Music", "Visible", true);
    m_settings["Widgets-Main.Visible"] = reader.GetBoolean("Widgets-Main", "Visible", true);
    m_settings["Widgets-Drives.Visible"] = reader.GetBoolean("Widgets-Drives", "Visible", true);
    m_settings["Widgets-FPS.Visible"] = reader.GetBoolean("Widgets-FPS", "Visible", true);
    m_settings["Widgets-NetStats.Visible"] = reader.GetBoolean("Widgets-NetStats", "Visible", true);
    m_settings["Widgets-CPUGraph.Visible"] = reader.GetBoolean("Widgets-CPUGraph", "Visible", true);
    m_settings["Widgets-RAMGraph.Visible"] = reader.GetBoolean("Widgets-RAMGraph", "Visible", true);
    m_settings["Widgets-NetGraph.Visible"] = reader.GetBoolean("Widgets-NetGraph", "Visible", true);
    m_settings["Widgets-GPUGraph.Visible"] = reader.GetBoolean("Widgets-GPUGraph", "Visible", true);

    m_settings["Widgets-ProcessesCPU.Position"] = reader.Get("Widgets-ProcessesCPU", "Position", "top-middle");
    m_settings["Widgets-ProcessesRAM.Position"] = reader.Get("Widgets-ProcessesRAM", "Position", "top-middle");
    m_settings["Widgets-Time.Position"] = reader.Get("Widgets-Time", "Position", "top-left");
    m_settings["Widgets-SystemStats.Position"] = reader.Get("Widgets-SystemStats", "Position", "bottom-middle");
    m_settings["Widgets-Music.Position"] = reader.Get("Widgets-Music", "Position", "bottom-right");
    m_settings["Widgets-CPUStats.Position"] = reader.Get("Widgets-CPUStats", "Position", "middle-right");
    m_settings["Widgets-Drives.Position"] = reader.Get("Widgets-Drives", "Position", "top-right");
    m_settings["Widgets-Main.Position"] = reader.Get("Widgets-Main", "Position", "middle-middle");
    m_settings["Widgets-FPS.Position"] = reader.Get("Widgets-FPS", "Position", "bottom-left");
    m_settings["Widgets-NetStats.Position"] = reader.Get("Widgets-NetStats", "Position", "bottom-middle");
    m_settings["Widgets-CPUGraph.Position"] = reader.Get("Widgets-CPUGraph", "Position", "middle-left");
    m_settings["Widgets-RAMGraph.Position"] = reader.Get("Widgets-RAMGraph", "Position", "middle-left");
    m_settings["Widgets-NetGraph.Position"] = reader.Get("Widgets-NetGraph", "Position", "middle-left");
    m_settings["Widgets-GPUGraph.Position"] = reader.Get("Widgets-GPUGraph", "Position", "middle-left");
}

} // namespace rg
