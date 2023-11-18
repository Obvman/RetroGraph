module Utils;

import std.filesystem;

import "CSTDHeaderUnit.h";
import "RGAssert.h";
import "WindowsHeaderUnit.h";

namespace rg {

std::string wstrToStr(const std::wstring& wstr) {
    if (wstr.empty())
        return {};

    const auto size_needed{ WideCharToMultiByte(CP_UTF8, 0, &wstr[0],
                                                static_cast<int>(wstr.size()),
                                                nullptr, 0, nullptr, nullptr) };

    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), &strTo[0],
                        size_needed, nullptr, nullptr);
    return strTo;
}

std::wstring strToWstr(const std::string& str) {
    if (str.empty())
        return {};

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
    WCHAR buff[MAX_PATH];
    GetModuleFileNameW(nullptr, buff, MAX_PATH);

    std::filesystem::path exeFileName{ buff };
    return exeFileName.remove_filename().string();
}

const std::string getExpandedEnvPath(const std::string& path) {
    CHAR buff[MAX_PATH];
    ExpandEnvironmentStringsA(path.c_str(), buff, MAX_PATH);
    return std::string{ buff };
}

void printTimeToExecuteMs(const char* funcName, std::regular_invocable auto f) {
    const auto start{ clock() };
    f();
    const auto end{ clock() };

    printf("%s took %f seconds\n", funcName, (static_cast<float>(end) - static_cast<float>(start)) / CLOCKS_PER_SEC);
}

void printTimeToExecuteMs(std::regular_invocable auto f) {
    printTimeToExecuteMs("Function", f);
}

void printTimeToExecuteHighRes(const char* funcName, std::regular_invocable auto f) {
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    const int64_t start{ li.QuadPart };

    f();

    QueryPerformanceCounter(&li);
    const int64_t end{ li.QuadPart };

    printf("%s took %I64d counts\n", funcName, end - start);
}

void printTimeToExecuteHighRes(std::regular_invocable auto f) {
    printTimeToExecuteHighRes("Function", f);
}

} // namespace rg
