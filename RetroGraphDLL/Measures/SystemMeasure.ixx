module;

#include "RGAssert.h"

export module Measures.SystemMeasure;

import Colors;
import Utils;
import Units;
import Measures.Measure;

import std.core;

import "WindowsHeaders.h";

#pragma comment(lib, "version.lib")

namespace rg {

export class SystemMeasure;

/* Contains static information about the computer so no need to update */
class SystemMeasure : public Measure {
public:
    SystemMeasure();
    ~SystemMeasure() noexcept = default;

    void update(int ticks) override;

    /* Returns string of current operating system version/build number */
    std::string_view getOSInfoStr() const { return m_osInfoStr; }

    /* Returns string of CPU information: Manufacturer, model, default clock
     * speed, architecture and core count */
    std::string_view getCPUDescription() const { return m_cpuDescription; }

    /* Returns string with RAM capacity */
    std::string_view getRAMDescription() const { return m_ramDescription; }

    /* Returns the windows user name of current user */
    std::string_view getUserName() const { return m_userName; }

    std::string_view getComputerName() const { return m_computerName; }

    void refreshSettings() override { }
private:
    bool shouldUpdate(int /*ticks*/) const override { return false; }

    /* Sets the contents of m_osInfoStr. Only needs to be called once */
    void getOSVersionInfo();
    /* Sets the contents of m_cpuDescription. Only needs to be called once */
    void getCPUInfo();
    /* Sets the contents of m_ramDescription. Only needs to be called once */
    void getRAMInfo();

    std::string m_osInfoStr{ "" };
    std::string m_cpuDescription{ "" };
    std::string m_ramDescription{ "" };
    std::string m_userName{ "" };
    std::string m_computerName{ "" };
};


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

void SystemMeasure::update(int /*ticks*/) {
}

void SystemMeasure::getOSVersionInfo() {
    // Use kernel32.dll's meta information to get the OS version
    const char* filePath{ "kernel32.dll" };

    DWORD dummy;
    const auto fileVersionInfoSize{ GetFileVersionInfoSize(filePath, &dummy) };
    auto* lpVersionInfo = new BYTE[fileVersionInfoSize];
    RGVERIFY(GetFileVersionInfo(filePath, 0, fileVersionInfoSize, lpVersionInfo), "Could not get OS version\n");

    UINT uLen;
    VS_FIXEDFILEINFO* lpFfi;
    const auto bVer{ VerQueryValue(lpVersionInfo, "\\", (LPVOID*)&lpFfi, &uLen) };
    RGASSERT(bVer && uLen != 0, "Failed to query OS value\n");

    const DWORD osVersionMS = lpFfi->dwProductVersionMS;
    const DWORD osVersionLS = lpFfi->dwProductVersionLS;
    delete[] lpVersionInfo;

    const DWORD dwLeftMost = HIWORD(osVersionMS);
    const DWORD dwSecondLeft = LOWORD(osVersionMS);
    const DWORD dwSecondRight = HIWORD(osVersionLS);
    const DWORD dwRightMost = LOWORD(osVersionLS);

    m_osInfoStr = std::format("Windows Version: {}.{}.{}.{}", dwLeftMost, dwSecondLeft, dwSecondRight, dwRightMost);
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
