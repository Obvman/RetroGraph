#pragma once

#include <string>
#include <Windows.h>

namespace rg {

constexpr uint64_t KB{ 1024 };
constexpr uint64_t MB{ KB * 1024 };
constexpr uint64_t GB{ MB * 1024 };

constexpr uint32_t maxTicks{ 100000U };
constexpr uint32_t tickDuration{ 100U }; // tick length in milliseconds
constexpr uint32_t ticksPerSecond{ 1000U / tickDuration };

/* Lines drawn too close to the edge of the window will clip so this value
   is used to separate drawing bounds from the very edge */
constexpr float bDelta{ 0.0001f };

/* Minimum distance from the edge of the screen to draw each object (in pixels) */
constexpr uint32_t marginX{ 16U };
constexpr uint32_t marginY{ 10U };

/* Displays an Error message box with the given string as a message */
void showMessageBox(const std::string& s);

/* Displays an Error message box with the given string as a message and
   exits the program with a failure code */
void fatalMessageBox(const std::string& s);

/* Converts FILETIME to an unsigned LARGE_INTEGER struct */
ULARGE_INTEGER ftToULI(const FILETIME& ft);

/* Subtracts the FILETIMES and returns result as 64 bit unsigned integer */
uint64_t subtractTimes(const FILETIME& ftA, const FILETIME& ftB);

/* When called after a viewport has been set, draws a white outline of the viewport */
void drawViewportBorder();

}