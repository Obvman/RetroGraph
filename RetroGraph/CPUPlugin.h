#pragma once

#include <stdint.h>
#include <CoreTempSDK/GetCoreTempInfo.h>

namespace rg {

/*  unsigned int	uiLoad[256];
	unsigned int	uiTjMax[128];
	unsigned int	uiCoreCnt;
	unsigned int	uiCPUCnt;
	float			fTemp[256];
	float			fVID;
	float			fCPUSpeed;
	float			fFSBSpeed;
	float			fMultipier;
	char			sCPUName[100];
	unsigned char	ucFahrenheit;
	unsigned char	ucDeltaToTjMax;*/

class CPUPlugin {
public:
    CPUPlugin();
    ~CPUPlugin();

    void update();

    uint32_t getNumCores() const { return m_ctData.uiCoreCnt; }
    float getVoltage() const { return m_ctData.fVID; }
    float getClockSpeed() const { return m_ctData.fCPUSpeed; }
    const char* getCPUName() const { return m_ctData.sCPUName; }
    float getTemp(uint32_t coreNum) const { return m_ctData.fTemp[coreNum]; }
    uint32_t getLoad(uint32_t coreNum) const { return m_ctData.uiLoad[coreNum]; }

    /* Returns true if the latest update managed to successfully fill the
       CORE_TEMP_SHARED_DATA struct */
    bool getCoreTempInfoSuccess() const { return m_getCoreTempInfoSuccess; }
private:
    CORE_TEMP_SHARED_DATA m_ctData;
    mutable bool m_getCoreTempInfoSuccess;
};

}