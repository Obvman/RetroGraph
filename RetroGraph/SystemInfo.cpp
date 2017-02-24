#include "SystemInfo.h"

#include <Winver.h>
#include <iostream>
#include <sstream>
#include <GL/gl.h>

#include "utils.h"

#pragma comment(lib, "version.lib")

namespace rg {

SystemInfo::SystemInfo() :
    m_osInfoStr{},
    m_gpuDescription{},
    m_gpuInfoAttained{ false } {

    getOSVersionInfo();
}


SystemInfo::~SystemInfo() {
}

void SystemInfo::getOSVersionInfo() {
    const char* filePath{ "kernel32.dll" };

    DWORD dummy;
    const DWORD fileVersionInfoSize{ GetFileVersionInfoSize(filePath, &dummy) };
    LPBYTE lpVersionInfo = new BYTE[fileVersionInfoSize];
    if (!GetFileVersionInfo(filePath, 0, fileVersionInfoSize, lpVersionInfo)) {
        fatalMessageBox("Could not get OS version\n");
    }

    UINT uLen;
    VS_FIXEDFILEINFO* lpFfi;
    const BOOL bVer = VerQueryValue(lpVersionInfo, "\\", (LPVOID*)&lpFfi, &uLen);
    if (!bVer || uLen == 0) {
        fatalMessageBox("Failed to query OS value\n");
    }

    const DWORD osVersionMS = lpFfi->dwProductVersionMS;
    const DWORD osVersionLS = lpFfi->dwProductVersionLS;
    delete[] lpVersionInfo;

    const DWORD dwLeftMost = HIWORD(osVersionMS);
    const DWORD dwSecondLeft = LOWORD(osVersionMS);
    const DWORD dwSecondRight = HIWORD(osVersionLS);
    const DWORD dwRightMost = LOWORD(osVersionLS);

    m_osInfoStr = std::string{ "Windows Version: " + std::to_string(dwLeftMost) + "." + std::to_string(dwSecondLeft)+ "." +
                               std::to_string(dwSecondRight)+ "." + std::to_string(dwRightMost)};
}

const std::string& SystemInfo::getGPUDescription() {
    if (!m_gpuInfoAttained) {
        getGPUInfo();
    }
    return m_gpuDescription;
}

void SystemInfo::getGPUInfo() {
    // Use a stringstream because glGetString() returns GLubyte* which is messy
    // to deal with otherwise
    const auto gpuVendor{ glGetString(GL_VENDOR) };
    const auto gpuRenderer{ glGetString(GL_RENDERER) };

    if (!gpuVendor || !gpuRenderer) {
        fatalMessageBox("Failed to query GPU information from OpenGL\n");
    }

    std::stringstream ss;
    ss << gpuRenderer << ", " << gpuVendor;
    m_gpuDescription = "GPU: " + ss.str();

    m_gpuInfoAttained = true;
}

}