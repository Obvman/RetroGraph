#pragma once

#include "stdafx.h"

#include <charconv>
#include <cstdint>
#include <string>
#include <string_view>
#include <concepts>

#include <Windows.h>

#include "units.h"

namespace rg {

/* Lines drawn too close to the edge of the window will clip so this value
   is used to separate drawing bounds from the very edge */
constexpr float bDelta{ 0.0001f };

/* Displays an Error message box with the given string as a message */
void showMessageBox(std::string_view s);

/* Displays an Error message box with the given string as a message and
   exits the program with a failure code */
void fatalMessageBox(std::string_view s);

/* Gets the directory of the executable file of this program */
const std::string getExePath();

/* Expands environment variables in path */
const std::string getExpandedEnvPath(const std::string& path);

/* Converts a wchar_t string to regular char string */
std::string wstrToStr(const std::wstring& wstr);

/* Converts a char string to wchar_t string */
std::wstring strToWstr(const std::string& str);

/* Converts FILETIME to an unsigned LARGE_INTEGER struct */
ULARGE_INTEGER ftToULI(const FILETIME& ft);

/* Subtracts the FILETIMES and returns result as 64 bit unsigned integer */
uint64_t subtractTimes(const FILETIME& ftA, const FILETIME& ftB);

/* Linear interpolation */
constexpr inline float lerp(float x1, float x2, float t) {
    return (1 - t) * x1 + t * x2;
}

/* Checks if the current tick is in line with the frequency given
   Use this if you want to perform an action [hz] times per second */
constexpr inline bool ticksMatchRate(int ticks, int hz) {
    return (ticks % static_cast<int>(static_cast<float>(rg::ticksPerSecond)/hz + 0.5)) == 0;
}

/* Checks if the current tick is in line with the number of seconds given
   Use this if you want to perform an action every [s] seconds */
constexpr inline bool ticksMatchSeconds(int ticks, int s) {
    return (ticks % (rg::ticksPerSecond * s)) == 0;
}

/* Prints how long the given function f took to execute */
void printTimeToExecuteMs(const char* funcName, std::regular_invocable auto f) {
    const auto start{ clock() };
    f();
    const auto end{ clock() };

    printf("%s took %f seconds\n", funcName, (static_cast<float>(end) - static_cast<float>(start)) / CLOCKS_PER_SEC);
}

/* Default function name overload */
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

template<typename T>
T strToNum(std::string_view str) {
    T val;
    const auto ret{ std::from_chars(str.data(), str.data() + str.size(), val) };

    if (ret.ec == std::errc::invalid_argument)
        throw std::invalid_argument("strToNum failed");
    else if (ret.ec == std::errc::result_out_of_range)
        throw std::out_of_range("strToNum failed");

    return val;
}

// Throws error if expr is false
inline void rgAssert(bool expr, std::string_view str) {
    if (!expr)
        showMessageBox(str);
}

// Throws error
inline void rgError(std::string_view str) {
    showMessageBox(str);
}

inline void rgVerify(bool expr [[maybe_unused]], std::string_view str[[maybe_unused]]) {
    if constexpr (debugMode)
        if (!expr)
            showMessageBox(str);
}

// Debug Assertions
#ifdef _DEBUG
#define RGASSERT(expr, str) rgAssert(expr, str)
#define RGERROR(str) rgError(str)
#else
#define RGASSERT(expr, str)
#define RGERROR(str)
#endif

// Throws error if expr is false. Always evaluates expr - even in release mode
#define RGVERIFY(expr, str) rgVerify(expr, str)

} // namespace rg
