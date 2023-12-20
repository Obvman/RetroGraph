module RG.Measures:SystemMeasure;

import RG.Core;

import "RGAssert.h";
import "WindowsHeaderUnit.h";

#pragma comment(lib, "version.lib")

namespace rg {

SystemMeasure::SystemMeasure(std::unique_ptr<const IRAMDataSource> ramDataSource)
    : Measure{ std::nullopt }
    , m_ramDataSource{ std::move(ramDataSource) }
    , m_ramDescription{ getRAMInfo(*m_ramDataSource) } {

    getOSVersionInfo();
    getCPUInfo();

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

void SystemMeasure::getOSVersionInfo() {
    // Use kernel32.dll's meta information to get the OS version
    const char* filePath{ "kernel32.dll" };

    DWORD dummy;
    const auto fileVersionInfoSize{ GetFileVersionInfoSize(filePath, &dummy) };
    std::vector<BYTE> versionInfoBuff(fileVersionInfoSize);
    RGVERIFY(GetFileVersionInfo(filePath, 0, fileVersionInfoSize, versionInfoBuff.data()), "Could not get OS version\n");

    UINT uLen;
    VS_FIXEDFILEINFO* lpFfi;
    const auto bVer{ VerQueryValue(versionInfoBuff.data(), "\\", (LPVOID*)&lpFfi, &uLen) };
    RGASSERT(bVer && uLen != 0, "Failed to query OS value\n");

    const DWORD osVersionMS = lpFfi->dwProductVersionMS;
    const DWORD osVersionLS = lpFfi->dwProductVersionLS;

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

std::string SystemMeasure::getRAMInfo(const IRAMDataSource& ramDataSource) const {
    char buff[64];
    snprintf(buff, sizeof(buff), "RAM: %2.1fGB", ramDataSource.getRAMCapacity() / static_cast<float>(GB));
    return buff;
}

} // namespace rg
