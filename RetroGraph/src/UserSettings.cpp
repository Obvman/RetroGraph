#include "../headers/UserSettings.h"

#include <iostream>
#include <string>
#include <boost/property_tree/ini_parser.hpp>
#include <Windows.h>

#include "../headers/utils.h"

namespace rg {

const std::string iniPath{ ((IsDebuggerPresent()) ? "resources\\config.ini" : "..\\RetroGraph\\resources\\config.ini") };

struct MonitorInfo {
    MonitorInfo(int16_t _index) : index{ _index }, handle{ nullptr } {}

    int16_t index{ -1 };
    HMONITOR handle{ nullptr };
};

BOOL CALLBACK MonitorCallback(HMONITOR hMonitor, HDC hdc, LPRECT lpRect, LPARAM dwData) {
    static uint16_t monCount{ 0 };

    auto info = (MonitorInfo*)dwData;

    if (info->index == monCount) {
        info->handle = hMonitor;
    }

    ++monCount;
    return TRUE;
}


UserSettings::UserSettings() :
    m_windowWidth{ 1920U },
    m_windowHeight{ 1170U },
    m_startupMonitor{ 0U },
    m_netAdapterName{ "Intel(R) Ethernet Connection (2) I219-V"} {
}

UserSettings::~UserSettings() {
}

void UserSettings::init() {
    pt::ptree propTree;
    try {
        pt::ini_parser::read_ini(iniPath, propTree);
        if (propTree.empty()) {
            generateDefaultFile(propTree);
        } else {
            m_windowWidth = propTree.get<uint16_t>("Window.WindowWidth");
            m_windowHeight = propTree.get<uint16_t>("Window.WindowHeight");
            m_startupMonitor = propTree.get<uint16_t>("Window.Monitor");
            m_netAdapterName = propTree.get<std::string>("Network.NetworkAdapter");
        }


    } catch (const pt::ptree_bad_path& e) {
        std::string errorMsg{ "Config file parsing error: " };
        errorMsg.append(e.what());
        fatalMessageBox(errorMsg);
    } catch (const pt::ini_parser_error&) {
        generateDefaultFile(propTree);
    }

    // Get handle to the monitor that the window will be created on
    MonitorInfo info{ m_startupMonitor };
    if (!EnumDisplayMonitors(nullptr, nullptr, MonitorCallback, (LPARAM)&info)) {
        fatalMessageBox("Failed to enumerate monitors");
    }
    if (!info.handle) {
        fatalMessageBox("Failed to get handle for monitor " + std::to_string(m_startupMonitor));
    }

    // If the window height/width values are 0, get the workspace area of the
    // monitor as the window resolution
    if (m_windowWidth == 0U || m_windowHeight == 0U) {
        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(info.handle, &monitorInfo);


        m_windowHeight = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;
        m_windowWidth = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
        m_createWindowX = monitorInfo.rcWork.left;
        m_createWindowY = monitorInfo.rcWork.top;
    }

}

void UserSettings::generateDefaultFile(pt::ptree& propTree) {
    std::cout << "Generating new config file\n";

    // Use default values for each property
    propTree.put("Window.WindowWidth", m_windowWidth);
    propTree.put("Window.WindowHeight", m_windowHeight);
    propTree.put("Window.Monitor", m_startupMonitor);

    propTree.put("Network.NetworkAdapter", m_netAdapterName);

    pt::ini_parser::write_ini(iniPath, propTree);
}

}