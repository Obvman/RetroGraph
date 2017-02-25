#include "SystemInfo.h"

#include <Winver.h>
#include <sstream>
#include <GL/gl.h>

#include "utils.h"
#include "colors.h"

#pragma comment(lib, "version.lib")

namespace rg {

SystemInfo::SystemInfo(GLint vpX, GLint vpY, GLint vpW, GLint vpH) :
    m_osInfoStr{},
    m_gpuDescription{},
    m_cpuDescription{},
    m_ramDescription{},
    m_viewportStartX{ vpX },
    m_viewportStartY{ vpY },
    m_viewportWidth{ vpW },
    m_viewportHeight{ vpH } {

    getOSVersionInfo();
    getCPUInfo();
    getRAMInfo();
}


SystemInfo::~SystemInfo() {
}

void SystemInfo::draw() const {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    glViewport(m_viewportStartX, m_viewportStartY, m_viewportWidth, m_viewportHeight);

    drawText();
    drawViewportBorder();

    glViewport(vp[0], vp[1], vp[2], vp[3]);
}

void SystemInfo::drawText() const {
    static constexpr auto numLines{ 4U };
    const auto rasterX = float{ -0.95f };
    auto rasterY = float{ 1.0f };

    glColor3f(TEXT_R, TEXT_G, TEXT_B);

    glRasterPos2f(rasterX, rasterY);
    glCallLists(m_osInfoStr.length(), GL_UNSIGNED_BYTE, m_osInfoStr.c_str());
    rasterY -= 1.0f / numLines;

    glRasterPos2f(rasterX, rasterY);
    glCallLists(m_gpuDescription.length(), GL_UNSIGNED_BYTE, m_gpuDescription.c_str());
    rasterY -= 1.0f / numLines;

    glRasterPos2f(rasterX, rasterY);
    glCallLists(m_cpuDescription.length(), GL_UNSIGNED_BYTE, m_cpuDescription.c_str());
    rasterY -= 1.0f / numLines;

    glRasterPos2f(rasterX, rasterY);
    glCallLists(m_ramDescription.length(), GL_UNSIGNED_BYTE, m_ramDescription.c_str());
    rasterY -= 1.0f / numLines;
}

void SystemInfo::getOSVersionInfo() {
    const char* filePath{ "kernel32.dll" };

    DWORD dummy;
    const DWORD fileVersionInfoSize{ GetFileVersionInfoSize(filePath, &dummy) };
    LPBYTE lpVersionInfo = new BYTE[fileVersionInfoSize];
    if (!GetFileVersionInfo(filePath, 0, fileVersionInfoSize, lpVersionInfo)) {
        fatalMessageBox("Could not get OS version\n");
    }

    UINT uLen;
    VS_FIXEDFILEINFO* lpFfi;
    const BOOL bVer = VerQueryValue(lpVersionInfo, "\\", (LPVOID*)&lpFfi, &uLen);
    if (!bVer || uLen == 0) {
        fatalMessageBox("Failed to query OS value\n");
    }

    const DWORD osVersionMS = lpFfi->dwProductVersionMS;
    const DWORD osVersionLS = lpFfi->dwProductVersionLS;
    delete[] lpVersionInfo;

    const DWORD dwLeftMost = HIWORD(osVersionMS);
    const DWORD dwSecondLeft = LOWORD(osVersionMS);
    const DWORD dwSecondRight = HIWORD(osVersionLS);
    const DWORD dwRightMost = LOWORD(osVersionLS);

    m_osInfoStr = std::string{ "Windows Version: " + std::to_string(dwLeftMost) + "." + std::to_string(dwSecondLeft)+ "." +
                               std::to_string(dwSecondRight)+ "." + std::to_string(dwRightMost)};
}

void SystemInfo::getCPUInfo() {
    SYSTEM_INFO info;
    GetSystemInfo(&info);

    std::string cpuArchitecture{};
    switch (info.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64:
            cpuArchitecture = "x64";
            break;
        case PROCESSOR_ARCHITECTURE_ARM:
            cpuArchitecture = "ARM";
            break;
        case PROCESSOR_ARCHITECTURE_IA64:
            cpuArchitecture = "Intel Itanium";
            break;
        case PROCESSOR_ARCHITECTURE_INTEL:
            cpuArchitecture = "x86";
            break;
        default:
            cpuArchitecture.append("Unkown CPU Architecture");
    }

    // Credit to bsruth - http://stackoverflow.com/questions/850774/how-to-determine-the-hardware-cpu-and-ram-on-a-machine
    int32_t CPUInfo[4] = {-1};
    // Get the information associated with each extended ID.
    __cpuid(CPUInfo, 0x80000000);
    const auto nExIds = CPUInfo[0];
    char CPUBrandString[0x40];
    for (int32_t i = 0x80000000; i <= nExIds; ++i) {
        __cpuid(CPUInfo, i);
        // Interpret CPU brand string
        if  (i == 0x80000002)
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
        else if  (i == 0x80000003)
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        else if  (i == 0x80000004)
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
    }
    //string includes manufacturer, model and clockspeed

    std::stringstream ss;
    ss  << "CPU: " << CPUBrandString << " (" << cpuArchitecture << "), "
        << info.dwNumberOfProcessors << " cores";
    m_cpuDescription = ss.str();
}

void SystemInfo::getRAMInfo() {
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memStatus);

    m_ramDescription = "RAM: " +
        std::to_string(memStatus.ullTotalPhys / (1024 * 1024)) + "MB";
}

const std::string& SystemInfo::getGPUDescription() {
    getGPUInfo();
    return m_gpuDescription;
}

void SystemInfo::getGPUInfo() {
    // Use a stringstream because glGetString() returns GLubyte* which is messy
    // to deal with otherwise
    const auto gpuVendor{ glGetString(GL_VENDOR) };
    const auto gpuRenderer{ glGetString(GL_RENDERER) };

    if (!gpuVendor || !gpuRenderer) {
        fatalMessageBox("Failed to query GPU information from OpenGL\n");
    }

    std::stringstream ss;
    ss << gpuRenderer << ", " << gpuVendor;
    m_gpuDescription = "GPU: " + ss.str();
}

}