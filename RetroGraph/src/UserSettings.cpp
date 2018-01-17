#include "../headers/UserSettings.h"

#include <iostream>
#include <string>
#include <boost/property_tree/ini_parser.hpp>
#include <Windows.h>
#include <map>

#include "../headers/utils.h"

namespace rg {

const std::string iniPath{ ((IsDebuggerPresent()) ? "resources\\config.ini" :
                            "..\\RetroGraph\\resources\\config.ini") };

UserSettings::UserSettings() {
    namespace po = boost::program_options;
    namespace pt = boost::property_tree;

    m_widgetVisibilities.assign(RG_NUM_WIDGETS, false);
    m_widgetPositions.resize(RG_NUM_WIDGETS);

    pt::ptree propTree;
    try {
        pt::ini_parser::read_ini(iniPath, propTree);
        if (propTree.empty()) {
            generateDefaultFile(propTree);
        } else {
            const std::map<std::string, WidgetPosition> posMap = {
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

            m_settings["Window.Monitor"] = propTree.get<int32_t>("Window.Monitor");

            m_settings["Window.ClickThrough"] = propTree.get<bool>("Window.ClickThrough");
            m_settings["Window.WidgetBackground"] = propTree.get<bool>("Window.WidgetBackground");

            m_settings["Network.PingServer"] = propTree.get<std::string>("Network.PingServer");
            m_settings["Network.PingFrequency"] = propTree.get<uint32_t>("Network.PingFrequency");

            m_settings[ "Widgets-ProcessesCPU.NumProcessesDisplayed"] = propTree.get<uint32_t>(
                "Widgets-ProcessesCPU.NumProcessesDisplayed");
            m_settings[ "Widgets-ProcessesCPU.HighCPUUsageThreshold"] = propTree.get<float>(
                "Widgets-ProcessesCPU.HighCPUUsageThreshold");

            m_settings[ "Widgets-ProcessesRAM.NumProcessesDisplayed"] = propTree.get<uint32_t>(
                "Widgets-ProcessesRAM.NumProcessesDisplayed");
            m_settings[ "Widgets-ProcessesRAM.HighRAMUsageThresholdMB"] = propTree.get<uint32_t>(
                "Widgets-ProcessesRAM.HighRAMUsageThresholdMB");

            m_settings["Widgets-Graphs-Network.NumUsageSamples"] = propTree.get<uint32_t>(
                "Widgets-Graphs-Network.NumUsageSamples");
            m_settings["Widgets-Graphs-CPU.NumUsageSamples"] = propTree.get<uint32_t>(
                "Widgets-Graphs-CPU.NumUsageSamples");
            m_settings["Widgets-Graphs-GPU.NumUsageSamples"] = propTree.get<uint32_t>(
                "Widgets-Graphs-GPU.NumUsageSamples");
            m_settings["Widgets-Graphs-RAM.NumUsageSamples"] = propTree.get<uint32_t>(
                "Widgets-Graphs-RAM.NumUsageSamples");

            m_widgetVisibilities[RG_WIDGET_TIME] = propTree.get<bool>(
                    "Widgets-Time.Visible");
            m_widgetVisibilities[RG_WIDGET_CPU_STATS] = propTree.get<bool>(
                    "Widgets-CPUStats.Visible");
            m_widgetVisibilities[RG_WIDGET_SYSTEM_STATS] = propTree.get<bool>(
                    "Widgets-SystemStats.Visible");
            m_widgetVisibilities[RG_WIDGET_PROCESSES_CPU] = propTree.get<bool>(
                    "Widgets-ProcessesCPU.Visible");
            m_widgetVisibilities[RG_WIDGET_PROCESSES_RAM] = propTree.get<bool>(
                    "Widgets-ProcessesRAM.Visible");
            m_widgetVisibilities[RG_WIDGET_MUSIC] = propTree.get<bool>(
                    "Widgets-Music.Visible");
            m_widgetVisibilities[RG_WIDGET_MAIN] = propTree.get<bool>(
                    "Widgets-Main.Visible");
            m_widgetVisibilities[RG_WIDGET_DRIVES] = propTree.get<bool>(
                    "Widgets-Drives.Visible");
            m_widgetVisibilities[RG_WIDGET_GRAPHS] = propTree.get<bool>(
                    "Widgets-Graphs.Visible");

            m_widgetPositions[RG_WIDGET_PROCESSES_CPU] = 
                    posMap.at(propTree.get<std::string>("Widgets-ProcessesCPU.Position"));
            m_widgetPositions[RG_WIDGET_PROCESSES_RAM] = 
                    posMap.at(propTree.get<std::string>("Widgets-ProcessesRAM.Position"));
            m_widgetPositions[RG_WIDGET_TIME] = 
                    posMap.at(propTree.get<std::string>("Widgets-Time.Position"));
            m_widgetPositions[RG_WIDGET_SYSTEM_STATS] = 
                    posMap.at(propTree.get<std::string>("Widgets-SystemStats.Position"));
            m_widgetPositions[RG_WIDGET_MUSIC] = 
                    posMap.at(propTree.get<std::string>("Widgets-Music.Position"));
            m_widgetPositions[RG_WIDGET_CPU_STATS] = 
                    posMap.at(propTree.get<std::string>("Widgets-CPUStats.Position"));
            m_widgetPositions[RG_WIDGET_DRIVES] = 
                    posMap.at(propTree.get<std::string>("Widgets-Drives.Position"));
            m_widgetPositions[RG_WIDGET_MAIN] = 
                    posMap.at(propTree.get<std::string>("Widgets-Main.Position"));
            m_widgetPositions[RG_WIDGET_GRAPHS] = 
                    posMap.at(propTree.get<std::string>("Widgets-Graphs.Position"));
        }

    } catch (const pt::ptree_bad_path& e) {
        std::string errorMsg{ "Config file parsing error: " };
        errorMsg.append(e.what());
        fatalMessageBox(errorMsg);
    } catch (const pt::ini_parser_error&) {
        generateDefaultFile(propTree);
    }
}

settingVariant UserSettings::getSettingValue(const std::string& settingName) const {
    if (m_settings.count(settingName) == 0) {
        fatalMessageBox("Failed to find setting " + settingName);
    }
    return m_settings.at(settingName);
}

void UserSettings::generateDefaultFile(boost::property_tree::ptree& propTree) {
    std::cout << "Generating new config file\n";

    // Use default values for each property
    propTree.put("Window.Monitor", std::get<int32_t>(m_settings["Window.Monitor"]));
    propTree.put("Window.ClickThrough", m_clickthrough);
    propTree.put("Window.WidgetBackground", m_widgetBackground);

    propTree.put("Network.PingServer", m_pingServer);
    propTree.put("Network.PingFrequency", m_pingServer);

    propTree.put("Widgets-ProcessesCPU.NumProcessesDisplayed",
            m_numCPUProcessesDisplayed);
    propTree.put("Widgets-ProcessesRAM.NumProcessesDisplayed",
            m_numRAMProcessesDisplayed);
    propTree.put("Widgets-ProcessesCPU.HighCPUUsageThreshold",
            m_processCPUUsageThreshold);
    propTree.put("Widgets-ProcessesRAM.HighRAMUsageThresholdMB",
            m_processRAMUsageThresholdMB);

    propTree.put("Widgets-Graphs-Network.NetUsageSamples", m_pingServer);
    propTree.put("Widgets-Graphs-CPU.CPUUsageSamples", m_pingServer);
    propTree.put("Widgets-Graphs-GPU.GPUUsageSamples", m_pingServer);
    propTree.put("Widgets-Graphs-RAM.RAMUsageSamples", m_pingServer);

    // Widget Visibilities
    propTree.put("Widgets-Time.Visible", isVisible(RG_WIDGET_TIME));
    propTree.put("Widgets-SystemStats.Visible", isVisible(RG_WIDGET_SYSTEM_STATS));
    propTree.put("Widgets-CPUStats.Visible", isVisible(RG_WIDGET_CPU_STATS));
    propTree.put("Widgets-ProcessesCPU.Visible", isVisible(RG_WIDGET_PROCESSES_CPU));
    propTree.put("Widgets-ProcessesRAM.Visible", isVisible(RG_WIDGET_PROCESSES_RAM));
    propTree.put("Widgets-Music.Visible", isVisible(RG_WIDGET_MUSIC));
    propTree.put("Widgets-Main.Visible", isVisible(RG_WIDGET_MAIN));
    propTree.put("Widgets-Drives.Visible", isVisible(RG_WIDGET_DRIVES));
    propTree.put("Widgets-Graphs.Visible", isVisible(RG_WIDGET_GRAPHS));

    // Widget Positions
    propTree.put("Widgets-Time.Position", "top-left");
    propTree.put("Widgets-SystemStats.Position", "bottom-left");
    propTree.put("Widgets-CPUStats.Position", "middle-right");
    propTree.put("Widgets-ProcessesCPU.Position", "bottom-middle");
    propTree.put("Widgets-ProcessesRAM.Position", "bottom-middle");
    propTree.put("Widgets-Music.Position", "bottom-right");
    propTree.put("Widgets-Main.Position", "middle-middle");
    propTree.put("Widgets-Drives.Position", "top-right");
    propTree.put("Widgets-Graphs.Position", "middle-left");

    boost::property_tree::ini_parser::write_ini(iniPath, propTree);
}

}
