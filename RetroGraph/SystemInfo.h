#pragma once

#include <string>
#include <Windows.h>

namespace rg {

class SystemInfo {
public:
    SystemInfo();
    ~SystemInfo();

    const std::string& getOSInfoStr() const { return m_osInfoStr; }
    // Must be called after OpenGL context has been created
    const std::string& getGPUDescription();
    const std::string& getCPUDescription() { return m_cpuDescription; }
    const std::string& getRAMDescription() { return m_ramDescription; }
private:
    void getOSVersionInfo();
    void getGPUInfo();
    void getCPUInfo();
    void getRAMInfo();

    std::string m_osInfoStr;
    std::string m_gpuDescription;
    std::string m_cpuDescription;
    std::string m_ramDescription;

    // flag determining whether we've already set the GPU info string so we
    // don't do it again unnecessarily when calling getGPUDescription()
    mutable bool m_gpuInfoAttained;
};

}
