module;

#include "RGAssert.h"

export module CPUPlugin;

import Utils;

import std.core;

import "WindowsHeaders.h";

namespace rg {

export class CPUPlugin;

using getCoreTempInfoFn = bool (WINAPI *)(CORE_TEMP_SHARED_DATA* pData);
getCoreTempInfoFn GetCoreTempInfo;

/* CPUPlugin interfaces with CoreTemp's shared memory to get more detailed
   statistics on the system's CPU: Clock speed, core count, voltage, model name,
   individual core temperatures and loads */
class CPUPlugin {
public:
    CPUPlugin();
    ~CPUPlugin() noexcept;
    CPUPlugin(const CPUPlugin&) = delete;
    CPUPlugin& operator=(const CPUPlugin&) = delete;
    CPUPlugin(CPUPlugin&&) = delete;
    CPUPlugin& operator=(CPUPlugin&&) = delete;

    /* Gets latest information from CoreTemp */
    void update();

    /* Returns the number of cores in the system's CPU */
    int getNumCores() const { return m_ctData.uiCoreCnt; }

    /* Returns the voltage running into the CPU in volts */
    float getVoltage() const { return m_ctData.fVID; }

    /* Returns the clock speed of the CPU in MHz */
    float getClockSpeed() const { return m_ctData.fCPUSpeed; }

    /* Returns the CPU Name */
    std::string getCPUName() const { return m_ctData.sCPUName; }

    /* Returns the current temperature of the given CPU core (Celsius) */
    float getTemp(int coreNum) const { return m_ctData.fTemp[coreNum]; }

    /* Returns the current load of the given core as an integer from 0-100 */
    int getLoad(unsigned int coreNum) const { return m_ctData.uiLoad[coreNum]; }

    /* Returns the maximum allowable CPU temperature in degrees celsius */
    int getTjMax() const { return m_ctData.uiTjMax[0]; }

    /* Returns true if the latest update managed to successfully fill the
       CORE_TEMP_SHARED_DATA struct */
    bool getCoreTempInfoSuccess() const { return m_getCoreTempInfoSuccess; }

    bool coreTempWasStarted() const { return m_coreTempWasStarted; }

private:
    HMODULE m_libHandle{ nullptr };
    CORE_TEMP_SHARED_DATA m_ctData{};
    bool m_coreTempWasStarted{ false };

    mutable bool m_getCoreTempInfoSuccess{ false };
};


CPUPlugin::CPUPlugin() : m_libHandle{ LoadLibrary("GetCoreTempInfo.dll") } {

	if (m_libHandle) {
		// Get the address of the GetCoreTempInfo function from the DLL
		GetCoreTempInfo = (getCoreTempInfoFn)GetProcAddress(m_libHandle, "fnGetCoreTempInfoAlt");
		if (GetCoreTempInfo) {
			// Fill the CORE_TEMP_SHARED_DATA struct with CPU stats from CoreTemp's
			// shared memory
			m_getCoreTempInfoSuccess = GetCoreTempInfo(&m_ctData);
		}
		else
			RGERROR("Could not get function address from GetCoreTempInfo.dll");
	}
	else
		RGERROR("Could not load GetCoreTempInfo.dll");
}

CPUPlugin::~CPUPlugin() noexcept {
	if (m_libHandle)
		FreeLibrary(m_libHandle);
}

void CPUPlugin::update() {
    m_coreTempWasStarted = false;

    /* If coreTemp failed to get information last frame but got it this frame,
     * then the program must have been started
     */
    const auto coreTempSuccessThisFrame = bool{ GetCoreTempInfo(&m_ctData) };
    if (!m_getCoreTempInfoSuccess && coreTempSuccessThisFrame) {
        m_getCoreTempInfoSuccess = true;
        m_coreTempWasStarted = true;
    } else {
        m_getCoreTempInfoSuccess = coreTempSuccessThisFrame;
    }
}

}
