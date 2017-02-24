#include "ProcessMeasure.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

#include <GL/freeglut.h>
#include <GL/gl.h>

#include "colors.h"
#include "ProcessData.h"
#include "utils.h"

namespace rg {

ProcessMeasure::ProcessMeasure(GLint vpX, GLint vpY, GLint vpW, GLint vpH) :
    m_processSnapshot{},
    m_allProcessData{},
    m_viewportStartX{ vpX },
    m_viewportStartY{ vpY },
    m_viewportWidth{ vpW },
    m_viewportHeight{ vpH } {
}


ProcessMeasure::~ProcessMeasure() {
}

void ProcessMeasure::init() {
    populateList();
}
// TODO benchmark using vector vs list

void ProcessMeasure::update(uint32_t ticks) {
    // Update the process list vector every 10 seconds
    if (ticks % 100 == 0) {
        std::cout << "Updating process list\n";
        m_allProcessData.clear();
        populateList();
    }

    // Track iterator outside while scope for std::erase
    auto it = m_allProcessData.begin();
    while (it != m_allProcessData.end()) {
        auto& pd = **it; // get reference to ProcessData for convenience

        // Get the process relating to the ProcessData object
        HANDLE pHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                     false, pd.getPID());
        if (!pHandle) {
            auto error = GetLastError();
            // If access is denied or the process is the system idle process, just silently skip the process
            if (error != ERROR_ACCESS_DENIED && pd.getPID() != 0) {
                fatalMessageBox("Failed to open process. Code: " + std::to_string(error));
            }
            continue;
        }

        DWORD exitCode{ 1UL };
        GetExitCodeProcess(pHandle, &exitCode);
        if (exitCode == 0 || exitCode == 1) {
            std::cout << "Removing process " << pd.getName() << " with exit code " << exitCode << "\n";

            it = m_allProcessData.erase(it);

            std::cout << "Vec Size: " << m_allProcessData.size() << '\n';
        } else {
            // Get new timing information and calculate the CPU usage
            const auto cpuUsage{ calculateCPUUsage(pHandle, pd) };
            pd.setCpuUsage(cpuUsage);

            ++it;
        }
    }

    // Sort the vector based on the current CPU usage of processes in descending order
    std::sort(m_allProcessData.begin(), m_allProcessData.end(),
    [](const auto& ppd1, const auto& ppd2) {
        return ppd1->getCpuUsage() > ppd2->getCpuUsage();
    });
}

void ProcessMeasure::draw() const {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    glViewport(m_viewportStartX, m_viewportStartY, m_viewportWidth, m_viewportHeight);

    drawUsageList();

    glViewport(vp[0], vp[1], vp[2], vp[3]);
}

void ProcessMeasure::drawUsageList() const {
    static constexpr uint8_t numProcessesToDisplay{ 7 };

    const auto rasterX = float{ -0.95f };
    auto rasterY = float{ 1.0f }; // Y changes for each process drawn

    glColor3f(TEXT_R, TEXT_G, TEXT_B);

    auto i{ 0U }; // counter for how many to draw
    for (const auto& ppd : m_allProcessData) {
        if (i >= numProcessesToDisplay) break;

        const auto& pd = *ppd;
        std::stringstream ss;
        ss << pd.getName() << ": " << std::setprecision(3) << std::lround(pd.getCpuUsage()) << "%";

        glRasterPos2f(rasterX, rasterY);
        for (const auto c : ss.str()) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }

        rasterY -= 1.0 / numProcessesToDisplay;
        ++i;
    }
}

double ProcessMeasure::calculateCPUUsage(HANDLE pHandle, ProcessData& pd) {
        // Find delta in the process's total CPU usage time
        FILETIME cTime;
        FILETIME eTime;
        FILETIME kTime;
        FILETIME uTime;
        GetProcessTimes(pHandle, &cTime, &eTime, &uTime, &kTime);
        const auto procKernelDiff{ subtractTimes(kTime, pd.getKernelTime()) };
        const auto procUserDiff{ subtractTimes(uTime, pd.getUserTime()) };
        const auto totalProc{ procKernelDiff + procUserDiff };

        // Find delta in the entire system's CPU usage time
        FILETIME sysIdle, sysKernel, sysUser;
        GetSystemTimes(&sysIdle, &sysKernel, &sysUser);
        const auto sysKernelDiff{ subtractTimes(sysKernel, pd.getLastSystemKernelTime()) };
        const auto sysUserDiff{ subtractTimes(sysUser, pd.getLastSystemUserTime()) };
        const auto totalSys{ sysKernelDiff + sysUserDiff };

        // Get the CPU usage as a percentage
        const double cpuUse = static_cast<double>(100 * totalProc) / static_cast<double>(totalSys);


        pd.setTimes(cTime, eTime, kTime, uTime);

        return cpuUse;
}

void ProcessMeasure::updateProcList() {

    // Get the system snapshot of processes handle
    m_processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (m_processSnapshot == INVALID_HANDLE_VALUE) {
        fatalMessageBox("Failed to get process snapshot.");
        exit(1);
    }

    // Get the first process from the snapshot
    PROCESSENTRY32 pe{};
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(m_processSnapshot, &pe)) {
        fatalMessageBox("Failed to get first process from snapshot.");
        CloseHandle(m_processSnapshot);
        exit(1);
    }

    CloseHandle(m_processSnapshot);
}


void ProcessMeasure::populateList() {
    m_allProcessData.clear();

    // Get the process snapshot
    m_processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (m_processSnapshot == INVALID_HANDLE_VALUE) {
        fatalMessageBox("Failed to get process snapshot.");
    }

    // Get the first process from the snapshot
    PROCESSENTRY32 pe{};
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(m_processSnapshot, &pe)) {
        CloseHandle(m_processSnapshot);
        fatalMessageBox("Failed to get first process from snapshot.");
    }

    // Iterate over the rest of the processes in the snapshot to fill vector
    do {
        auto pHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 
                                   false, pe.th32ProcessID);
        if (!pHandle) {
            const auto error = GetLastError();
            // If access is denied or the process is the system idle process, just silently skip the process
            if (error != ERROR_ACCESS_DENIED && pe.th32ProcessID != 0) {
                fatalMessageBox("Failed to open process. Code: " + std::to_string(error));
            }
            continue;
        }

        // Populate the vector
        m_allProcessData.emplace_back(std::make_unique<ProcessData>(pHandle, pe.th32ProcessID, pe.szExeFile));

    } while (Process32Next(m_processSnapshot, &pe));

    CloseHandle(m_processSnapshot);
}

}