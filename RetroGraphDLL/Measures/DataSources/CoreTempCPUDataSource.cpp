module RG.Measures.DataSources:CoreTempCPUDataSource;

import "CoreTempHeaderUnit.h";
import "RGAssert.h";

namespace rg {

using getCoreTempInfoFn = bool(WINAPI*)(CORE_TEMP_SHARED_DATA* pData);
getCoreTempInfoFn getCoreTempInfo;

CoreTempCPUDataSource::CoreTempCPUDataSource()
    : m_cpuName{}
    , m_libHandle{ LoadLibrary("GetCoreTempInfo.dll") }
    , m_ctData{} {
    if (m_libHandle) {
        // Get the address of the getCoreTempInfo function from the DLL
        getCoreTempInfo = (getCoreTempInfoFn)GetProcAddress(m_libHandle, "fnGetCoreTempInfoAlt");
        if (getCoreTempInfo) {
            // Fill the CORE_TEMP_SHARED_DATA struct with CPU stats from CoreTemp's
            // shared memory
            RGVERIFY(getCoreTempInfo(&m_ctData), "CoreTemp call failed.");
            m_cpuName = m_ctData.sCPUName;
        } else {
            RGERROR("Could not get function address from GetCoreTempInfo.dll");
        }
    } else {
        RGERROR("Could not load GetCoreTempInfo.dll");
    }
}

CoreTempCPUDataSource::~CoreTempCPUDataSource() noexcept {
    if (m_libHandle)
        FreeLibrary(m_libHandle);
}

void CoreTempCPUDataSource::update() {
    /* If coreTemp failed to get information last frame but got it this frame,
     * then the program must have been started
     */
    RGVERIFY(getCoreTempInfo(&m_ctData), "CoreTemp call failed.");
}

float CoreTempCPUDataSource::getCPUUsage() const {
    float averageCoreUsage{ 0.0f };
    for (int i{ 0 }; i < getNumCores(); ++i) {
        const auto coreUsage{ getCoreUsage(i) };
        averageCoreUsage += coreUsage;
    }
    averageCoreUsage /= getNumCores();
    return averageCoreUsage;
}

} // namespace rg
