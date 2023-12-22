module RG.Measures.DataSources:Win32OperatingSystemDataSource;

import "RGAssert.h";
import "WindowsHeaderUnit.h";

#pragma comment(lib, "version.lib")

namespace rg {

OperatingSystemData Win32OperatingSystemDataSource::getOperatingSystemData() const {
    OperatingSystemData data;
    data.osName = "Windows";
    data.osVersion = getOSVersion();

    // Get the current computer name
    char uNameBuf[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD uNameLen{ sizeof(uNameBuf) };
    GetUserName(uNameBuf, &uNameLen);
    data.userName = uNameBuf;

    // Get the computer name
    char cNameBuf[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD cNameLen{ sizeof(cNameBuf) };
    GetComputerName(cNameBuf, &cNameLen);
    data.computerName = cNameBuf;

    return data;
}

std::string Win32OperatingSystemDataSource::getOSVersion() const {
    // Use kernel32.dll's meta information to get the OS version
    const char* filePath{ "kernel32.dll" };

    DWORD dummy;
    const auto fileVersionInfoSize{ GetFileVersionInfoSize(filePath, &dummy) };
    std::vector<BYTE> versionInfoBuff(fileVersionInfoSize);
    RGVERIFY(GetFileVersionInfo(filePath, 0, fileVersionInfoSize, versionInfoBuff.data()),
             "Could not get OS version\n");

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

    return std::format("{}.{}.{}.{}", dwLeftMost, dwSecondLeft, dwSecondRight, dwRightMost);
}

} // namespace rg
