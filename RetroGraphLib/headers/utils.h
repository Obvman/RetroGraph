#pragma once

#include <cstdint>
#include <string>
#include <Windows.h>

namespace rg {


/* Lines drawn too close to the edge of the window will clip so this value
   is used to separate drawing bounds from the very edge */
constexpr float bDelta{ 0.0001f };

/* Displays an Error message box with the given string as a message */
void showMessageBox(const std::string& s);

/* Displays an Error message box with the given string as a message and
   exits the program with a failure code */
void fatalMessageBox(const std::string& s);

/* Converts a wchar_t string to regular char string */
std::string wstrToStr(const std::wstring& wstr);

/* Converts a char string to wchar_t string */
std::wstring strToWstr(const std::string& str);

/* Converts FILETIME to an unsigned LARGE_INTEGER struct */
ULARGE_INTEGER ftToULI(const FILETIME& ft);

/* Subtracts the FILETIMES and returns result as 64 bit unsigned integer */
uint64_t subtractTimes(const FILETIME& ftA, const FILETIME& ftB);

/* Linear interpolation */
constexpr float lerp(float x1, float x2, float t);

/* Prints how long the given function f took to execute */
template<typename F>
void printTimeToExecuteMs(const char* funcName, F f) {
    const auto start{ clock() };
    f();
    const auto end{ clock() };

    printf("%s took %f seconds\n", funcName, 
        (static_cast<float>(end) - static_cast<float>(start)) / CLOCKS_PER_SEC);
}

/* Default function name overload */
template<typename F>
void printTimeToExecuteMs(F f) {
    printTimeToExecuteMs("Function", f);
}

template<typename F>
void printTimeToExecuteHighRes(const char* funcName, F f) {
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    const int64_t start{ li.QuadPart };

    f();

    QueryPerformanceCounter(&li);
    const int64_t end{ li.QuadPart };

    printf("%s took %I64d counts\n", funcName, end - start);
}

template<typename F>
void printTimeToExecuteHighRes(F f) {
    printTimeToExecuteHighRes("Function", f);
}

} // namespace rg
