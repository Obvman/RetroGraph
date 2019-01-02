#include "stdafx.h"

#include "utils.h"

// #include <GL/glew.h>
#include <locale>

#include "colors.h"

namespace rg {

void showMessageBox(std::string_view s) {
    MessageBox(nullptr, std::string{ s }.c_str(), "Error", MB_OK | MB_ICONERROR);
}

void fatalMessageBox(std::string_view s) {
    showMessageBox(s);
    if constexpr (debugMode)
        DebugBreak();
    throw std::runtime_error(std::string{ s });
}

std::string wstrToStr(const std::wstring& wstr) {
    if(wstr.empty()) return std::string{ };

    const auto size_needed{ WideCharToMultiByte(
        CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr) };

    std::string strTo( size_needed, 0 );
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), &strTo[0], 
                        size_needed, nullptr, nullptr);
    return strTo;
}

std::wstring strToWstr(const std::string& str) {
    if(str.empty()) return std::wstring();
    const auto size_needed{ MultiByteToWideChar(CP_UTF8, 0, &str[0], 
                                                static_cast<int>(str.size()), nullptr, 0) };
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), 
                        &wstrTo[0], size_needed);
    return wstrTo;
}

ULARGE_INTEGER ftToULI(const FILETIME& ft) {
     ULARGE_INTEGER uli;
     uli.LowPart = ft.dwLowDateTime;
     uli.HighPart = ft.dwHighDateTime;

     return uli;
}

uint64_t subtractTimes(const FILETIME& ftA, const FILETIME& ftB) {
     LARGE_INTEGER a, b;
     a.LowPart = ftA.dwLowDateTime;
     a.HighPart = ftA.dwHighDateTime;

     b.LowPart = ftB.dwLowDateTime;
     b.HighPart = ftB.dwHighDateTime;

     return a.QuadPart - b.QuadPart;
}

const std::string getExePath() {
    WCHAR fBuff[MAX_PATH];
    GetModuleFileNameW(nullptr, fBuff, sizeof(fBuff));
    PathCchRemoveFileSpec(fBuff, sizeof(fBuff));

    std::wstring path{ fBuff };
    return wstrToStr(path);
}

const std::string getExpandedEnvPath(const std::string& path) {
    CHAR fBuff[MAX_PATH];
    ExpandEnvironmentStringsA(path.c_str(), fBuff, MAX_PATH);
    return std::string{ fBuff };
}

}
