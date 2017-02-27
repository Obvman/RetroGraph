#include "../headers/CPUPlugin.h"

#include "../headers/utils.h"

namespace rg {

typedef bool (WINAPI *myGetCoreTempInfo)(CORE_TEMP_SHARED_DATA* pData);
myGetCoreTempInfo GetCoreTempInfo;

CPUPlugin::CPUPlugin() :
    m_libHandle{ LoadLibrary("GetCoreTempInfo.dll") },
    m_ctData{ 0 },
    m_getCoreTempInfoSuccess{ false } {

    if (!m_libHandle) {
        fatalMessageBox("Could not load GetCoreTempInfo.dll");
    }

    // Get the address of the GetCoreTempInfo function from the DLL
    GetCoreTempInfo = (myGetCoreTempInfo)GetProcAddress(m_libHandle, "fnGetCoreTempInfoAlt");
    if (!GetCoreTempInfo) {
        fatalMessageBox("Could not get function address from GetCoreTempInfo.dll");
    }

    // Fill the CORE_TEMP_SHARED_DATA struct with CPU stats from CoreTemp's
    // shared memory
    m_getCoreTempInfoSuccess = GetCoreTempInfo(&m_ctData);
}


CPUPlugin::~CPUPlugin() {
    FreeLibrary(m_libHandle);
}

void CPUPlugin::update() {
    m_getCoreTempInfoSuccess = GetCoreTempInfo(&m_ctData);
}

}