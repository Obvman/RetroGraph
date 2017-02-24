#pragma once

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

/* Converts FILETIME to an unsigned LARGE_INTEGER struct */
ULARGE_INTEGER ftToULI(const FILETIME& ft);

/* Subtracts the FILETIMES and returns result as 64 bit unsigned integer */
uint64_t subtractTimes(const FILETIME& ftA, const FILETIME& ftB);

}