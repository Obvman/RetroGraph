#pragma once

#include <string>
#include <GL/glew.h>
#include <Windows.h>

#include "Measure.h"

namespace rg {

/* Contains static information about the computer so no need to update */
class SystemMeasure : public Measure {
public:
    SystemMeasure();
    virtual ~SystemMeasure();
    SystemMeasure(const SystemMeasure&) = delete;
    SystemMeasure& operator=(const SystemMeasure&) = delete;

    virtual void init() override;

    virtual void update(uint32_t ticks) override;

    /* Returns string of current operating system version/build number */
    const std::string& getOSInfoStr() const { return m_osInfoStr; }

    /* Sets the  string of GPU manufacturer and model information
     * Must be called after OpenGL context has been created */
    void updateGPUDescription();

    /* Gets the string containing GPU model */
    const std::string& getGPUDescription() const { return m_gpuDescription; }

    /* Returns string of CPU information: Manufacturer, model, default clock
     * speed, architecture and core count */
    const std::string& getCPUDescription() const { return m_cpuDescription; }

    /* Returns string with RAM capacity */
    const std::string& getRAMDescription() const { return m_ramDescription; }

    /* Returns the windows user name of current user */
    const std::string& getUserName() const { return m_userName; }

    /* Returns the computer's name */
    const std::string& getComputerName() const { return m_computerName; }

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
    std::string m_userName;
    std::string m_computerName;
};

}
