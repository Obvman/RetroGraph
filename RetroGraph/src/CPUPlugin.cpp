#include "../headers/CPUPlugin.h"

#include "../headers/utils.h"

namespace rg {

typedef bool (WINAPI *myGetCoreTempInfo)(CORE_TEMP_SHARED_DATA* pData);
myGetCoreTempInfo GetCoreTempInfo;

CPUPlugin::CPUPlugin() :
    m_libHandle{ nullptr },
    m_ctData{ 0 },
    m_getCoreTempInfoSuccess{ false },
    m_coreTempWasStarted{ false } {
}


CPUPlugin::~CPUPlugin() {
    FreeLibrary(m_libHandle);
}

void CPUPlugin::init() {
    m_libHandle = LoadLibrary("GetCoreTempInfo.dll");

    if (!m_libHandle) {
        fatalMessageBox("Could not load GetCoreTempInfo.dll");
    }

    // Get the address of the GetCoreTempInfo function from the DLL
    GetCoreTempInfo = (myGetCoreTempInfo)GetProcAddress(
        m_libHandle, "fnGetCoreTempInfoAlt"
    );
    if (!GetCoreTempInfo) {
        fatalMessageBox("Could not get function address from GetCoreTempInfo.dll");
    }

    // Fill the CORE_TEMP_SHARED_DATA struct with CPU stats from CoreTemp's
    // shared memory
    m_getCoreTempInfoSuccess = GetCoreTempInfo(&m_ctData);
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
