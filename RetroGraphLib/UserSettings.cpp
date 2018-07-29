#include "stdafx.h"

#include "UserSettings.h"

#include <Windows.h>
#include <pathcch.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include <inih/INIReader.h>

#include "utils.h"

#pragma comment(lib, "Pathcch.lib")

namespace rg {

const std::string UserSettings::iniPath{ getExePath() + R"(\..\..\resources\config.ini)" };
const std::string UserSettings::fallbackIniPath{ getExePath() + R"(\..\..\resources\default_config.ini)" };

UserSettings::UserSettings()
    : m_widgetVisibilities(Widgets::NumWidgets, true)
    , m_widgetPositions(Widgets::NumWidgets) {

    // TODO only read config if no settings file
    readConfig();

    writeDataFile();
}

void UserSettings::readConfig() {
    auto reader = INIReader{ iniPath };
    if (reader.ParseError() < 0) {
        showMessageBox("Failed to load config file from " + iniPath + "\nUsing default_config.ini");
        reader = INIReader{ fallbackIniPath };

        if (reader.ParseError() < 0) {
            fatalMessageBox("Failed to load default config file.");
        }
    }

    try {
        readMembers(reader);
    } catch (const std::exception& e) {
        auto errorMsg = std::string{ "Config file path error: " };
        errorMsg.append(e.what());
        fatalMessageBox(errorMsg);
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

    m_widgetVisibilities[Widgets::Time]        = reader.GetBoolean("Widgets-Time",         "Visible", true);
    m_widgetVisibilities[Widgets::CPUStats]    = reader.GetBoolean("Widgets-CPUStats",     "Visible", true);
    m_widgetVisibilities[Widgets::SystemStats] = reader.GetBoolean("Widgets-SystemStats",  "Visible", true);
    m_widgetVisibilities[Widgets::ProcessCPU]  = reader.GetBoolean("Widgets-ProcessesCPU", "Visible", true);
    m_widgetVisibilities[Widgets::ProcessRAM]  = reader.GetBoolean("Widgets-ProcessesRAM", "Visible", true);
    m_widgetVisibilities[Widgets::Music]       = reader.GetBoolean("Widgets-Music",        "Visible", true);
    m_widgetVisibilities[Widgets::Main]        = reader.GetBoolean("Widgets-Main",         "Visible", true);
    m_widgetVisibilities[Widgets::HDD]         = reader.GetBoolean("Widgets-Drives",       "Visible", true);
    m_widgetVisibilities[Widgets::FPS]         = reader.GetBoolean("Widgets-FPS",          "Visible", true);
    m_widgetVisibilities[Widgets::NetStats]    = reader.GetBoolean("Widgets-NetStats",     "Visible", true);
    m_widgetVisibilities[Widgets::CPUGraph]    = reader.GetBoolean("Widgets-CPUGraph",     "Visible", true);
    m_widgetVisibilities[Widgets::RAMGraph]    = reader.GetBoolean("Widgets-RAMGraph",     "Visible", true);
    m_widgetVisibilities[Widgets::NetGraph]    = reader.GetBoolean("Widgets-NetGraph",     "Visible", true);
    m_widgetVisibilities[Widgets::GPUGraph]    = reader.GetBoolean("Widgets-GPUGraph",     "Visible", true);

    m_widgetPositions[Widgets::ProcessCPU]  = m_posMap.at(reader.Get("Widgets-ProcessesCPU", "Position", "top-middle"));
    m_widgetPositions[Widgets::ProcessRAM]  = m_posMap.at(reader.Get("Widgets-ProcessesRAM", "Position", "top-middle"));
    m_widgetPositions[Widgets::Time]        = m_posMap.at(reader.Get("Widgets-Time",         "Position", "top-left"));
    m_widgetPositions[Widgets::SystemStats] = m_posMap.at(reader.Get("Widgets-SystemStats",  "Position", "bottom-middle"));
    m_widgetPositions[Widgets::Music]       = m_posMap.at(reader.Get("Widgets-Music",        "Position", "bottom-right"));
    m_widgetPositions[Widgets::CPUStats]    = m_posMap.at(reader.Get("Widgets-CPUStats",     "Position", "middle-right"));
    m_widgetPositions[Widgets::HDD]         = m_posMap.at(reader.Get("Widgets-Drives",       "Position", "top-right"));
    m_widgetPositions[Widgets::Main]        = m_posMap.at(reader.Get("Widgets-Main",         "Position", "middle-middle"));
    m_widgetPositions[Widgets::FPS]         = m_posMap.at(reader.Get("Widgets-FPS",          "Position", "bottom-left"));
    m_widgetPositions[Widgets::NetStats]    = m_posMap.at(reader.Get("Widgets-NetStats",     "Position", "bottom-middle"));
    m_widgetPositions[Widgets::CPUGraph]    = m_posMap.at(reader.Get("Widgets-CPUGraph",     "Position", "middle-left"));
    m_widgetPositions[Widgets::RAMGraph]    = m_posMap.at(reader.Get("Widgets-RAMGraph",     "Position", "middle-left"));
    m_widgetPositions[Widgets::NetGraph]    = m_posMap.at(reader.Get("Widgets-NetGraph",     "Position", "middle-left"));
    m_widgetPositions[Widgets::GPUGraph]    = m_posMap.at(reader.Get("Widgets-GPUGraph",     "Position", "middle-left"));
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
    if (!outFile) {
        fatalMessageBox("Couldn't write settings to file\n");
        return;
    }

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
        outFile << i << ' ' << m_widgetPositions[i] << '\n';
    }

}

}
