#include "../headers/UserSettings.h"

#include <iostream>
#include <string>
#include <boost/property_tree/ini_parser.hpp>
#include <Windows.h>
#include <map>

#include "../headers/utils.h"

namespace rg {

namespace po = boost::program_options;
namespace pt = boost::property_tree;


const std::string iniPath{ ((IsDebuggerPresent()) ? "resources\\config.ini" :
                            "..\\RetroGraph\\resources\\config.ini") };

UserSettings::UserSettings() {

    m_widgetVisibilities.assign(RG_NUM_WIDGETS, false);
    m_widgetPositions.resize(RG_NUM_WIDGETS);

    pt::ptree propTree;
    try {
        pt::ini_parser::read_ini(iniPath, propTree);
        if (propTree.empty()) {
            generateDefaultFile(propTree);
        } else {
            std::map<std::string, WidgetPosition> posMap = {
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

            m_startupMonitor = propTree.get<uint32_t>("Window.Monitor");
            m_clickthrough = propTree.get<bool>("Window.ClickThrough");
            m_widgetBackground = propTree.get<bool>("Window.WidgetBackground");

            m_pingServer = propTree.get<std::string>("Network.PingServer");
            m_pingFreq = propTree.get<uint32_t>("Network.PingFrequency");

            m_numCPUProcessesDisplayed = propTree.get<uint32_t>(
                    "Widgets-ProcessesCPU.NumProcessesDisplayed");
            m_processCPUUsageThreshold = propTree.get<float>(
                    "Widgets-ProcessesCPU.HighCPUUsageThreshold");

            m_numRAMProcessesDisplayed = propTree.get<uint32_t>(
                    "Widgets-ProcessesRAM.NumProcessesDisplayed");
            m_processRAMUsageThresholdMB = propTree.get<uint32_t>(
                    "Widgets-ProcessesRAM.HighRAMUsageThresholdMB");

            m_netUsageSamples = propTree.get<uint32_t>(
                    "Widgets-Graphs-Network.NumUsageSamples");
            m_cpuUsageSamples = propTree.get<uint32_t>(
                    "Widgets-Graphs-CPU.NumUsageSamples");
            m_gpuUsageSamples = propTree.get<uint32_t>(
                    "Widgets-Graphs-GPU.NumUsageSamples");
            m_ramUsageSamples = propTree.get<uint32_t>(
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
                    posMap[propTree.get<std::string>("Widgets-ProcessesCPU.Position")];
            m_widgetPositions[RG_WIDGET_PROCESSES_RAM] = 
                    posMap[propTree.get<std::string>("Widgets-ProcessesRAM.Position")];
            m_widgetPositions[RG_WIDGET_TIME] = 
                    posMap[propTree.get<std::string>("Widgets-Time.Position")];
            m_widgetPositions[RG_WIDGET_SYSTEM_STATS] = 
                    posMap[propTree.get<std::string>("Widgets-SystemStats.Position")];
            m_widgetPositions[RG_WIDGET_MUSIC] = 
                    posMap[propTree.get<std::string>("Widgets-Music.Position")];
            m_widgetPositions[RG_WIDGET_CPU_STATS] = 
                    posMap[propTree.get<std::string>("Widgets-CPUStats.Position")];
            m_widgetPositions[RG_WIDGET_DRIVES] = 
                    posMap[propTree.get<std::string>("Widgets-Drives.Position")];
            m_widgetPositions[RG_WIDGET_MAIN] = 
                    posMap[propTree.get<std::string>("Widgets-Main.Position")];
            m_widgetPositions[RG_WIDGET_GRAPHS] = 
                    posMap[propTree.get<std::string>("Widgets-Graphs.Position")];
        }

    } catch (const pt::ptree_bad_path& e) {
        std::string errorMsg{ "Config file parsing error: " };
        errorMsg.append(e.what());
        fatalMessageBox(errorMsg);
    } catch (const pt::ini_parser_error&) {
        generateDefaultFile(propTree);
    }
}

void UserSettings::generateDefaultFile(pt::ptree& propTree) {
    std::cout << "Generating new config file\n";

    // Use default values for each property
    propTree.put("Window.Monitor", m_startupMonitor);
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

    pt::ini_parser::write_ini(iniPath, propTree);
}

}
