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

    m_widgetVisibilities.assign(RG_NUM_WIDGETS, true);
    m_widgetPositions.resize(RG_NUM_WIDGETS);

    pt::ptree propTree;
    try {
        pt::ini_parser::read_ini(iniPath, propTree);
        if (propTree.empty()) {
            generateDefaultFile(propTree);
        } else {
            m_settings["Window.Monitor"] = 
                propTree.get<int32_t>("Window.Monitor");

            m_settings["Window.ClickThrough"] =
                propTree.get<bool>("Window.ClickThrough");
            m_settings["Window.WidgetBackground"] =
                propTree.get<bool>("Window.WidgetBackground");

            m_settings["Network.PingServer"] =
                propTree.get<std::string>("Network.PingServer");
            m_settings["Network.PingFrequency"] =
                propTree.get<uint32_t>("Network.PingFrequency");

            m_settings[ "Widgets-ProcessesCPU.NumProcessesDisplayed"] =
                propTree.get<uint32_t>("Widgets-ProcessesCPU.NumProcessesDisplayed");
            m_settings[ "Widgets-ProcessesCPU.HighCPUUsageThreshold"] =
                propTree.get<float>("Widgets-ProcessesCPU.HighCPUUsageThreshold");

            m_settings[ "Widgets-ProcessesRAM.NumProcessesDisplayed"] =
                propTree.get<uint32_t>("Widgets-ProcessesRAM.NumProcessesDisplayed");
            m_settings[ "Widgets-ProcessesRAM.HighRAMUsageThresholdMB"] =
                propTree.get<uint32_t>("Widgets-ProcessesRAM.HighRAMUsageThresholdMB");

            m_settings["Widgets-Graphs-Network.NumUsageSamples"] =
                propTree.get<uint32_t>("Widgets-Graphs-Network.NumUsageSamples");
            m_settings["Widgets-Graphs-CPU.NumUsageSamples"] =
                propTree.get<uint32_t>("Widgets-Graphs-CPU.NumUsageSamples");
            m_settings["Widgets-Graphs-GPU.NumUsageSamples"] =
                propTree.get<uint32_t>("Widgets-Graphs-GPU.NumUsageSamples");
            m_settings["Widgets-Graphs-RAM.NumUsageSamples"] =
                propTree.get<uint32_t>("Widgets-Graphs-RAM.NumUsageSamples");

            m_settings["Widgets-Main.FPS"] =
                propTree.get<uint32_t>("Widgets-Main.FPS");

            m_widgetVisibilities[RG_WIDGET_TIME] =
                propTree.get<bool>("Widgets-Time.Visible");
            m_widgetVisibilities[RG_WIDGET_CPU_STATS] =
                propTree.get<bool>("Widgets-CPUStats.Visible");
            m_widgetVisibilities[RG_WIDGET_SYSTEM_STATS] =
                propTree.get<bool>("Widgets-SystemStats.Visible");
            m_widgetVisibilities[RG_WIDGET_PROCESSES_CPU] =
                propTree.get<bool>("Widgets-ProcessesCPU.Visible");
            m_widgetVisibilities[RG_WIDGET_PROCESSES_RAM] =
                propTree.get<bool>("Widgets-ProcessesRAM.Visible");
            m_widgetVisibilities[RG_WIDGET_MUSIC] =
                propTree.get<bool>("Widgets-Music.Visible");
            m_widgetVisibilities[RG_WIDGET_MAIN] =
                propTree.get<bool>("Widgets-Main.Visible");
            m_widgetVisibilities[RG_WIDGET_DRIVES] =
                propTree.get<bool>("Widgets-Drives.Visible");
            m_widgetVisibilities[RG_WIDGET_GRAPHS] =
                propTree.get<bool>("Widgets-Graphs.Visible");

            m_widgetPositions[RG_WIDGET_PROCESSES_CPU] =
                    m_posMap.at(propTree.get<std::string>(
                                    "Widgets-ProcessesCPU.Position"));
            m_widgetPositions[RG_WIDGET_PROCESSES_RAM] = 
                    m_posMap.at(propTree.get<std::string>(
                                    "Widgets-ProcessesRAM.Position"));
            m_widgetPositions[RG_WIDGET_TIME] = 
                    m_posMap.at(propTree.get<std::string>(
                                    "Widgets-Time.Position"));
            m_widgetPositions[RG_WIDGET_SYSTEM_STATS] = 
                    m_posMap.at(propTree.get<std::string>(
                                    "Widgets-SystemStats.Position"));
            m_widgetPositions[RG_WIDGET_MUSIC] = 
                    m_posMap.at(propTree.get<std::string>(
                                    "Widgets-Music.Position"));
            m_widgetPositions[RG_WIDGET_CPU_STATS] = 
                    m_posMap.at(propTree.get<std::string>(
                                    "Widgets-CPUStats.Position"));
            m_widgetPositions[RG_WIDGET_DRIVES] = 
                    m_posMap.at(propTree.get<std::string>(
                                    "Widgets-Drives.Position"));
            m_widgetPositions[RG_WIDGET_MAIN] = 
                    m_posMap.at(propTree.get<std::string>(
                                    "Widgets-Main.Position"));
            m_widgetPositions[RG_WIDGET_GRAPHS] = 
                    m_posMap.at(propTree.get<std::string>(
                                    "Widgets-Graphs.Position"));
        }

    } catch (const pt::ptree_bad_path& e) {
        std::string errorMsg{ "Config file parsing error: " };
        errorMsg.append(e.what());
        fatalMessageBox(errorMsg);
    } catch (const pt::ini_parser_error&) {
        generateDefaultFile(propTree);
    }
}

settingVariant UserSettings::getVal(const std::string& settingName) const {
    if (m_settings.count(settingName) == 0) {
        fatalMessageBox("Failed to find setting " + settingName);
    }
    return m_settings.at(settingName);
}

void UserSettings::generateDefaultFile(boost::property_tree::ptree& propTree) {
    std::cout << "Generating new config file\n";

    m_settings = {
        { "Window.Monitor", 0 },
        { "Window.ClickThrough", true },
        { "Window.WidgetBackground", false },
        { "Network.PingServer", std::string{ "http://www.google.com/" } },
        { "Network.PingFrequency", 10U },
        { "Widgets-ProcessesCPU.NumProcessesDisplayed", 8U },
        { "Widgets-ProcessesRAM.NumProcessesDisplayed", 8U },
        { "Widgets-ProcessesCPU.HighCPUUsageThreshold", 0.2f },
        { "Widgets-ProcessesRAM.HighRAMUsageThresholdMB", 1024U },
        { "Widgets-Graphs-Network.NumUsageSamples", 20U },
        { "Widgets-Graphs-CPU.NumUsageSamples", 20U },
        { "Widgets-Graphs-GPU.NumUsageSamples", 20U },
        { "Widgets-Graphs-RAM.NumUsageSamples", 20U },
    };

    // Use default values for each property
    propTree.put("Window.Monitor", 
                 std::get<int32_t>(m_settings["Window.Monitor"]));
    propTree.put("Window.ClickThrough", 
                 std::get<bool>(m_settings["Window.ClickThrough"]));
    propTree.put("Window.WidgetBackground", 
                 std::get<bool>(m_settings["Window.WidgetBackground"]));

    propTree.put("Network.PingServer", 
                 std::get<std::string>(m_settings["Network.PingServer"]));
    propTree.put("Network.PingFrequency", 
                 std::get<uint32_t>(m_settings["Network.PingFrequency"]));

    propTree.put("Widgets-ProcessesCPU.NumProcessesDisplayed", 
                 std::get<uint32_t>(
                    m_settings["Widgets-ProcessesCPU.NumProcessesDisplayed"]));
    propTree.put("Widgets-ProcessesRAM.NumProcessesDisplayed", 
                 std::get<uint32_t>(
                    m_settings["Widgets-ProcessesRAM.NumProcessesDisplayed"]));
    propTree.put("Widgets-ProcessesCPU.HighCPUUsageThreshold", 
                 std::get<float>(
                    m_settings["Widgets-ProcessesCPU.HighCPUUsageThreshold"]));
    propTree.put("Widgets-ProcessesRAM.HighRAMUsageThresholdMB", 
                 std::get<uint32_t>(
                    m_settings["Widgets-ProcessesRAM.HighRAMUsageThresholdMB"]));

    propTree.put("Widgets-Graphs-Network.NumUsageSamples", 
                 std::get<uint32_t>(
                    m_settings["Widgets-Graphs-Network.NumUsageSamples"]));
    propTree.put("Widgets-Graphs-CPU.NumUsageSamples", 
                 std::get<uint32_t>(
                    m_settings["Widgets-Graphs-CPU.NumUsageSamples"]));
    propTree.put("Widgets-Graphs-GPU.NumUsageSamples", 
                 std::get<uint32_t>(
                    m_settings["Widgets-Graphs-GPU.NumUsageSamples"]));
    propTree.put("Widgets-Graphs-RAM.NumUsageSamples", 
                 std::get<uint32_t>(
                    m_settings["Widgets-Graphs-RAM.NumUsageSamples"]));

    propTree.put("Widgets-Main.FPS",
                 std::get<uint32_t>(m_settings["Widgets-Main.FPS"]));

    // Widget Visibilities
    propTree.put("Widgets-Time.Visible",         isVisible(RG_WIDGET_TIME));
    propTree.put("Widgets-SystemStats.Visible",  isVisible(RG_WIDGET_SYSTEM_STATS));
    propTree.put("Widgets-CPUStats.Visible",     isVisible(RG_WIDGET_CPU_STATS));
    propTree.put("Widgets-ProcessesCPU.Visible", isVisible(RG_WIDGET_PROCESSES_CPU));
    propTree.put("Widgets-ProcessesRAM.Visible", isVisible(RG_WIDGET_PROCESSES_RAM));
    propTree.put("Widgets-Music.Visible",        isVisible(RG_WIDGET_MUSIC));
    propTree.put("Widgets-Main.Visible",         isVisible(RG_WIDGET_MAIN));
    propTree.put("Widgets-Drives.Visible",       isVisible(RG_WIDGET_DRIVES));
    propTree.put("Widgets-Graphs.Visible",       isVisible(RG_WIDGET_GRAPHS));

    // Widget Positions
    propTree.put("Widgets-Time.Position",         "top-left");
    propTree.put("Widgets-SystemStats.Position",  "bottom-left");
    propTree.put("Widgets-CPUStats.Position",     "middle-right");
    propTree.put("Widgets-ProcessesCPU.Position", "bottom-middle");
    propTree.put("Widgets-ProcessesRAM.Position", "bottom-middle");
    propTree.put("Widgets-Music.Position",        "bottom-right");
    propTree.put("Widgets-Main.Position",         "middle-middle");
    propTree.put("Widgets-Drives.Position",       "top-right");
    propTree.put("Widgets-Graphs.Position",       "middle-left");

    m_widgetPositions[RG_WIDGET_PROCESSES_CPU] = 
        m_posMap.at(propTree.get<std::string>("Widgets-ProcessesCPU.Position"));
    m_widgetPositions[RG_WIDGET_PROCESSES_RAM] = 
        m_posMap.at(propTree.get<std::string>("Widgets-ProcessesRAM.Position"));
    m_widgetPositions[RG_WIDGET_TIME] = 
        m_posMap.at(propTree.get<std::string>("Widgets-Time.Position"));
    m_widgetPositions[RG_WIDGET_SYSTEM_STATS] = 
        m_posMap.at(propTree.get<std::string>("Widgets-SystemStats.Position"));
    m_widgetPositions[RG_WIDGET_MUSIC] = 
        m_posMap.at(propTree.get<std::string>("Widgets-Music.Position"));
    m_widgetPositions[RG_WIDGET_CPU_STATS] = 
        m_posMap.at(propTree.get<std::string>("Widgets-CPUStats.Position"));
    m_widgetPositions[RG_WIDGET_DRIVES] = 
        m_posMap.at(propTree.get<std::string>("Widgets-Drives.Position"));
    m_widgetPositions[RG_WIDGET_MAIN] = 
        m_posMap.at(propTree.get<std::string>("Widgets-Main.Position"));
    m_widgetPositions[RG_WIDGET_GRAPHS] = 
        m_posMap.at(propTree.get<std::string>("Widgets-Graphs.Position"));

    boost::property_tree::ini_parser::write_ini(iniPath, propTree);
}

}
