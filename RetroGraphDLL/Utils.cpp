module Utils;

import std.filesystem;

import "RGAssert.h";
import "WindowsHeaderUnit.h";

namespace rg {

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

} // namespace rg
