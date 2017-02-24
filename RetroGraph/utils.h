#pragma once

#include <string>
#include <Windows.h>

namespace rg {
// Must draw the lines a slight distance from the window edge otherwise they
// won't show up
constexpr float bDelta{ 0.0001f };

void showMessageBox(const std::string& s);

void fatalMessageBox(const std::string& s);

ULARGE_INTEGER ftToULI(const FILETIME& ft);

uint64_t subtractTimes(const FILETIME& ftA, const FILETIME& ftB);

}