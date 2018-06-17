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

#include <cstdint>
#include <cassert>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <string>
#include <locale>
#include <vector>
#include <array>
#include <map>
#include <variant>
#include <memory>
#include <thread>
#include <atomic>
#include <utility>

#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

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

#include "d3dkmt.h"