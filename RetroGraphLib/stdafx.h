#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <Windowsx.h>
#include <Winver.h>
#include <intrin.h>
#include <dwmapi.h>
#include <Psapi.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <winternl.h>
#include <pathcch.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <algorithm>
#include <array>
#include <atomic>
#include <concepts>
#include <chrono>
#include <charconv>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

#include <inih/INIReader.h>

#include <CoreTempSDK/GetCoreTempInfo.h>

#include <NVAPI/nvapi.h>

// NOTE: We can't #include these in source files since there are limitations to include orders
// If you're going to use any OpenGL library, put a commented include in the file to make the
// dependency clear

#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/wglew.h>
#include <GL/gl.h>

namespace rg {
#ifdef _DEBUG
constexpr auto debugMode = bool{ true };
#else
constexpr auto debugMode = bool{ false };
#endif
}