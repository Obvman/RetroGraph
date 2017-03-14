#pragma once

#include <stdint.h>
#include <string>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>

namespace rg {

namespace po = boost::program_options;
namespace pt = boost::property_tree;

class UserSettings {
public:
    UserSettings();
    ~UserSettings();

    void init();

    uint32_t getWindowWidth() const { return m_windowWidth; }
    uint32_t getWindowHeight() const { return m_windowHeight; }
    int32_t getWindowX() const { return m_createWindowX; }
    int32_t getWindowY() const { return m_createWindowY; }
    int32_t getStartupMonitor() const { return m_startupMonitor; }

    const std::string& getNetAdapterName() const { return m_netAdapterName; }
    const std::string& getPingServer() const { return m_pingServer; }
private:
    void generateDefaultFile(pt::ptree& propTree);

    // Window options
    uint32_t m_windowHeight;
    uint32_t m_windowWidth;
    int32_t m_createWindowX;
    int32_t m_createWindowY;
    int32_t m_startupMonitor;

    // Network options
    std::string m_netAdapterName;
    std::string m_pingServer;
};

}
