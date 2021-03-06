#include "stdafx.h"

#include "SystemMeasure.h"

#include <Winver.h>
#include <intrin.h>
#include <sstream>
// #include <GL/gl.h>

#include "utils.h"
#include "units.h"
#include "colors.h"

#pragma comment(lib, "version.lib")

namespace rg {

SystemMeasure::SystemMeasure() {
    getOSVersionInfo();
    getCPUInfo();
    getRAMInfo();

    // Get the current computer name
    char uNameBuf[MAX_COMPUTERNAME_LENGTH+1];
    DWORD uNameLen{ sizeof(uNameBuf) };
    GetUserName(uNameBuf, &uNameLen);
    m_userName.append(uNameBuf);

    // Get the computer name
    char cNameBuf[MAX_COMPUTERNAME_LENGTH+1];
    DWORD cNameLen{ sizeof(cNameBuf) };
    GetComputerName(cNameBuf, &cNameLen);
    m_computerName.append(cNameBuf);
}

void SystemMeasure::update(int ticks) {
    (void)ticks;
}

void SystemMeasure::getOSVersionInfo() {
    // Use kernel32.dll's meta information to get the OS version
    const char* filePath{ "kernel32.dll" };

    DWORD dummy;
    const auto fileVersionInfoSize{ GetFileVersionInfoSize(filePath, &dummy) };
    LPBYTE lpVersionInfo = new BYTE[fileVersionInfoSize];
    if (!GetFileVersionInfo(filePath, 0, fileVersionInfoSize, lpVersionInfo)) {
        fatalMessageBox("Could not get OS version\n");
    }

    UINT uLen;
    VS_FIXEDFILEINFO* lpFfi;
    const auto bVer{ VerQueryValue(lpVersionInfo,
                                   "\\",
                                   (LPVOID*)&lpFfi,
                                   &uLen) };
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

    m_osInfoStr = std::string{ "Windows Version: " + std::to_string(dwLeftMost)
                               + "." + std::to_string(dwSecondLeft)+ "." +
                               std::to_string(dwSecondRight)+ "." +
                               std::to_string(dwRightMost) };
}

void SystemMeasure::getCPUInfo() {

    // Credit to bsruth -
    // http://stackoverflow.com/questions/850774/how-to-determine-the-hardware-cpu-and-ram-on-a-machine
    int CPUInfo[4] = {-1};
    // Get the information associated with each extended ID.
    __cpuid(CPUInfo, 0x80000000);
    const auto nExIds{ CPUInfo[0] };
    char CPUBrandString[0x40];
    for (int i = 0x80000000; i <= nExIds; ++i) {
        __cpuid(CPUInfo, i);
        // Interpret CPU brand string
        if  (i == 0x80000002)
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
        else if  (i == 0x80000003)
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        else if  (i == 0x80000004)
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
    }

    m_cpuDescription = "CPU: " + std::string{ CPUBrandString };
}

void SystemMeasure::getRAMInfo() {
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memStatus);

    char buff[12];
    snprintf(buff, sizeof(buff), "RAM: %2.1fGB",
             memStatus.ullTotalPhys/static_cast<float>(GB));
    m_ramDescription = buff;
}
}
