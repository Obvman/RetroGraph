#include "../headers/UserSettings.h"

#include <iostream>
#include <string>
#include <boost/property_tree/ini_parser.hpp>
#include <Windows.h>

#include "../headers/utils.h"

namespace rg {

const std::string iniPath{ ((IsDebuggerPresent()) ? "resources\\config.ini" : "..\\RetroGraph\\resources\\config.ini") };

UserSettings::UserSettings() {
    pt::ptree propTree;
    try {
        pt::ini_parser::read_ini(iniPath, propTree);
        if (propTree.empty()) {
            generateDefaultFile(propTree);
        } else {
            m_windowWidth = propTree.get<uint32_t>("Window.WindowWidth");
            m_windowHeight = propTree.get<uint32_t>("Window.WindowHeight");
            m_startupMonitor = propTree.get<uint32_t>("Window.Monitor");
            m_pingServer = propTree.get<std::string>("Network.PingServer");
            m_pingFreq = propTree.get<uint32_t>("Network.PingFrequency");

            m_numProcessesDisplayed = propTree.get<uint32_t>(
                    "Processes.NumProcessesDisplayed");
            m_processCPUUsageThreshold = propTree.get<float>(
                    "Processes.HighCPUUsageThreshold");
            m_processRAMUsageThresholdMB = propTree.get<uint32_t>(
                    "Processes.HighRAMUsageThresholdMB");
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
    propTree.put("Window.WindowWidth", m_windowWidth);
    propTree.put("Window.WindowHeight", m_windowHeight);
    propTree.put("Window.Monitor", m_startupMonitor);

    propTree.put("Network.PingServer", m_pingServer);
    propTree.put("Network.PingFrequency", m_pingServer);

    propTree.put("Processes.NumProcessesDisplayed", m_numProcessesDisplayed);
    propTree.put("Processes.HighCPUUsageThreshold", m_processCPUUsageThreshold);
    propTree.put("Processes.HighRAMUsageThresholdMB", m_processRAMUsageThresholdMB);

    pt::ini_parser::write_ini(iniPath, propTree);
}

}
