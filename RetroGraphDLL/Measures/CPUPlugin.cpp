module Measures.CPUPlugin;

import "CoreTempHeaderUnit.h";
import "RGAssert.h";

namespace rg {

using getCoreTempInfoFn = bool (WINAPI *)(CORE_TEMP_SHARED_DATA* pData);
getCoreTempInfoFn GetCoreTempInfo;

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

} // namespace rg
