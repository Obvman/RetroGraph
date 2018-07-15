#include "stdafx.h"

#include "UserSettings.h"

#include <iostream>
#include <string>
#include <Windows.h>
#include <pathcch.h>
#include <map>

#include <inih/INIReader.h>

#include "utils.h"

#pragma comment(lib, "Pathcch.lib")

namespace rg {

const std::string UserSettings::iniPath{ getExePath() + R"(\..\..\resources\config.ini)" };
const std::string UserSettings::fallbackIniPath{ getExePath() + R"(\..\..\resources\default_config.ini)" };

UserSettings::UserSettings()
    : m_widgetVisibilities(Widgets::NumWidgets, true)
    , m_widgetPositions(Widgets::NumWidgets) {

    readConfig();
}

void UserSettings::readConfig() {
    INIReader reader{ iniPath };
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
        std::string errorMsg{ "Config file path error: " };
        errorMsg.append(e.what());
        fatalMessageBox(errorMsg);
    }
}

void UserSettings::readMembers(const INIReader& reader) {
    m_settings["Application.AutoReadConfig"]                   = reader.GetBoolean("Application", "AutoReadConfig", true);
    m_settings["Window.Monitor"]                               = reader.GetInteger("Window", "Monitor", 0);
    m_settings["Window.ClickThrough"]                          = reader.GetBoolean("Window", "ClickThrough", false);
    m_settings["Window.WidgetBackground"]                      = reader.GetBoolean("Window", "WidgetBackground", true);
    m_settings["Network.PingServer"]                           = reader.Get       ("Network", "PingServer", "http://www.google.com/");
    m_settings["Network.PingFrequency"]                        = reader.GetInteger("Network", "PingFrequency", 10);
    m_settings["Widgets-ProcessesCPU.NumProcessesDisplayed"]   = reader.GetInteger("Widgets-ProcessesCPU", "NumProcessesDisplayed", 10);
    m_settings["Widgets-ProcessesCPU.HighCPUUsageThreshold"]   = reader.GetReal   ("Widgets-ProcessesCPU", "HighCPUUsageThreshold", 0.2);
    m_settings["Widgets-ProcessesRAM.NumProcessesDisplayed"]   = reader.GetInteger("Widgets-ProcessesRAM", "NumProcessesDisplayed", 10);
    m_settings["Widgets-ProcessesRAM.HighRAMUsageThresholdMB"] = reader.GetInteger("Widgets-ProcessesRAM", "HighRAMUsageThresholdMB", 1024);
    m_settings["Widgets-Graphs-Network.NumUsageSamples"]       = reader.GetInteger("Widgets-Graphs-Network", "NumUsageSamples", 40);
    m_settings["Widgets-Graphs-CPU.NumUsageSamples"]           = reader.GetInteger("Widgets-Graphs-CPU", "NumUsageSamples", 40);
    m_settings["Widgets-Graphs-GPU.NumUsageSamples"]           = reader.GetInteger("Widgets-Graphs-GPU", "NumUsageSamples", 40);
    m_settings["Widgets-Graphs-RAM.NumUsageSamples"]           = reader.GetInteger("Widgets-Graphs-RAM", "NumUsageSamples", 40);
    m_settings["Widgets-Main.FPS"]                             = reader.GetInteger("Widgets-Main", "FPS", 30);

    m_widgetVisibilities[Widgets::Time]        = reader.GetBoolean("Widgets-Time",         "Visible", true);
    m_widgetVisibilities[Widgets::CPUStats]    = reader.GetBoolean("Widgets-CPUStats",     "Visible", true);
    m_widgetVisibilities[Widgets::SystemStats] = reader.GetBoolean("Widgets-SystemStats",  "Visible", true);
    m_widgetVisibilities[Widgets::ProcessCPU]  = reader.GetBoolean("Widgets-ProcessesCPU", "Visible", true);
    m_widgetVisibilities[Widgets::ProcessRAM]  = reader.GetBoolean("Widgets-ProcessesRAM", "Visible", true);
    m_widgetVisibilities[Widgets::Music]       = reader.GetBoolean("Widgets-Music",        "Visible", true);
    m_widgetVisibilities[Widgets::Main]        = reader.GetBoolean("Widgets-Main",         "Visible", true);
    m_widgetVisibilities[Widgets::HDD]         = reader.GetBoolean("Widgets-Drives",       "Visible", true);
    m_widgetVisibilities[Widgets::Graph]       = reader.GetBoolean("Widgets-Graphs",       "Visible", true);
    m_widgetVisibilities[Widgets::FPS]         = reader.GetBoolean("Widgets-FPS",          "Visible", true);
    m_widgetVisibilities[Widgets::NetStats]    = reader.GetBoolean("Widgets-NetStats",     "Visible", true);

    m_widgetPositions[Widgets::ProcessCPU]  = m_posMap.at(reader.Get("Widgets-ProcessesCPU", "Position", "top-middle"));
    m_widgetPositions[Widgets::ProcessRAM]  = m_posMap.at(reader.Get("Widgets-ProcessesRAM", "Position", "top-middle"));
    m_widgetPositions[Widgets::Time]        = m_posMap.at(reader.Get("Widgets-Time",         "Position", "top-left"));
    m_widgetPositions[Widgets::SystemStats] = m_posMap.at(reader.Get("Widgets-SystemStats",  "Position", "bottom-middle"));
    m_widgetPositions[Widgets::Music]       = m_posMap.at(reader.Get("Widgets-Music",        "Position", "bottom-right"));
    m_widgetPositions[Widgets::CPUStats]    = m_posMap.at(reader.Get("Widgets-CPUStats",     "Position", "middle-right"));
    m_widgetPositions[Widgets::HDD]         = m_posMap.at(reader.Get("Widgets-Drives",       "Position", "top-right"));
    m_widgetPositions[Widgets::Main]        = m_posMap.at(reader.Get("Widgets-Main",         "Position", "middle-middle"));
    m_widgetPositions[Widgets::Graph]       = m_posMap.at(reader.Get("Widgets-Graphs",       "Position", "middle-left"));
    m_widgetPositions[Widgets::FPS]         = m_posMap.at(reader.Get("Widgets-FPS",          "Position", "bottom-left"));
    m_widgetPositions[Widgets::NetStats]    = m_posMap.at(reader.Get("Widgets-NetStats",     "Position", "bottom-middle"));
}

}
