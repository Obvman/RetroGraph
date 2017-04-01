#pragma once

#include <stdint.h>
#include <CoreTempSDK/GetCoreTempInfo.h>

namespace rg {

/* CPUPlugin interfaces with CoreTemp's shared memory to get more detailed
   statistics on the system's CPU: Clock speed, core count, voltage, model name,
   individual core temperatures and loads */
class CPUPlugin {
public:
    CPUPlugin();
    ~CPUPlugin() noexcept;
    CPUPlugin(const CPUPlugin&) = delete;
    CPUPlugin& operator=(const CPUPlugin&) = delete;

    /* Gets latest information from CoreTemp */
    void update();

    /* Returns the number of cores in the system's CPU */
    uint32_t getNumCores() const { return m_ctData.uiCoreCnt; }

    /* Returns the voltage running into the CPU in volts */
    float getVoltage() const { return m_ctData.fVID; }

    /* Returns the clock speed of the CPU in MHz */
    float getClockSpeed() const { return m_ctData.fCPUSpeed; }

    /* Returns the CPU Name */
    const char* getCPUName() const { return m_ctData.sCPUName; }

    /* Returns the current temperature of the given CPU core (Celsius) */
    float getTemp(uint32_t coreNum) const { return m_ctData.fTemp[coreNum]; }

    /* Returns the current load of the given core as an integer from 0-100 */
    uint32_t getLoad(uint32_t coreNum) const { return m_ctData.uiLoad[coreNum]; }

    /* Returns the maximum allowable CPU temperature in degrees celsius */
    uint32_t getTjMax() const { return m_ctData.uiTjMax[0]; }

    /* Returns true if the latest update managed to successfully fill the
       CORE_TEMP_SHARED_DATA struct */
    bool getCoreTempInfoSuccess() const { return m_getCoreTempInfoSuccess; }

    bool coreTempWasStarted() const { return m_coreTempWasStarted; }

private:
    HMODULE m_libHandle{ nullptr };
    CORE_TEMP_SHARED_DATA m_ctData{ 0 };
    mutable bool m_getCoreTempInfoSuccess{ false };
    bool m_coreTempWasStarted{ false };
};

}
