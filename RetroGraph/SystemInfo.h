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
private:
    void getOSVersionInfo();
    void getGPUInfo();

    std::string m_osInfoStr;
    std::string m_gpuDescription;

    // flag determining whether we've already set the GPU info string so we
    // don't do it again unnecessarily when calling getGPUDescription()
    mutable bool m_gpuInfoAttained;
};

}
