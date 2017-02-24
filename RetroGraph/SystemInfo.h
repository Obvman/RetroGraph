#pragma once

#include <string>
#include <Windows.h>

namespace rg {

class SystemInfo {
public:
    SystemInfo();
    ~SystemInfo();

    /* Returns string of current operating system version/build number */
    const std::string& getOSInfoStr() const { return m_osInfoStr; }

    /* Returns string of GPU manufacturer and model information
     * Must be called after OpenGL context has been created */
    const std::string& getGPUDescription();

    /* Returns string of CPU information: Manufacturer, model, default clock
     * speed, architecture and core count */
    const std::string& getCPUDescription() { return m_cpuDescription; }

    /* Returns string with RAM capacity */
    const std::string& getRAMDescription() { return m_ramDescription; }

private:
    /* Sets the contents of m_osInfoStr. Only needs to be called once */
    void getOSVersionInfo();
    /* Queries OpenGL for GPU description and fills m_gpuDescription with
     * GPU model and manufacturer information. Must be called after
     * OpenGL context is created and only needs to be called once */
    void getGPUInfo();
    /* Sets the contents of m_cpuDescription. Only needs to be called once */
    void getCPUInfo();
    /* Sets the contents of m_ramDescription. Only needs to be called once */
    void getRAMInfo();

    std::string m_osInfoStr;
    std::string m_gpuDescription;
    std::string m_cpuDescription;
    std::string m_ramDescription;
};

}
