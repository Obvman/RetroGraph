#include "../headers/ProcessMeasure.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <utility>

#include <GL/freeglut.h>
#include <GL/gl.h>

#include "../headers/colors.h"
#include "../headers/ProcessData.h"
#include "../headers/utils.h"

namespace rg {

ProcessMeasure::ProcessMeasure() :
    m_allProcessData{},
    m_numProcessesToDisplay{ 7 },
    m_procCPUListData{ m_numProcessesToDisplay },
    m_procRAMListData{ m_numProcessesToDisplay } {

}


ProcessMeasure::~ProcessMeasure() {
}

void ProcessMeasure::init() {
    populateList();
    fillRAMData();
}
// TODO benchmark using vector vs list

void ProcessMeasure::update(uint32_t ticks) {
    //std::vector<std::shared_ptr<ProcessData>> oldData;

    // Update the process list vector every 10 seconds
    if ((ticks % (ticksPerSecond * 10)) == 0) {
        /*oldData.reserve( m_allProcessData.size() );
        for (const auto& spd : m_allProcessData) {
            oldData.push_back(spd);
        }*/

        m_allProcessData.clear();
        populateList();
    }

    if ((ticks % (ticksPerSecond * 2)) == 0) {
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

            // Check for processes that have exited and remove them from the list
            DWORD exitCode{ 1UL };
            GetExitCodeProcess(pHandle, &exitCode);
            if (exitCode == 0 || exitCode == 1) {
                it = m_allProcessData.erase(it);
            } else {
                /*if (oldData.size() > 0) {
                    auto found{ std::find_if(m_allProcessData.begin(), m_allProcessData.end(),
                                             [&pd](const auto& p) {
                        return pd.getPID() == p->getPID();
                    }) };

                    if (found != m_allProcessData.end()) {
                        // Use the old data to calculate CPU usage instead
                        const auto cpuUsage{ calculateCPUUsage(pHandle, **found) };

                        pd.setCpuUsage(cpuUsage);
                        pd.updateMemCounters();
                    }

                } else*/ {
                    // Get new timing information and calculate the CPU usage
                    const auto cpuUsage{ calculateCPUUsage(pHandle, pd) };
                    pd.setCpuUsage(cpuUsage);
                    pd.updateMemCounters();
                }


                ++it;
            }
        }

        fillCPUData();
    }

    if ((ticks % (ticksPerSecond * 5)) == 0) {
        fillRAMData();
    }
}


void ProcessMeasure::fillCPUData() {
    // Sort the vector based on the current CPU usage of processes in descending order
    std::sort(m_allProcessData.begin(), m_allProcessData.end(),
    [](const auto& ppd1, const auto& ppd2) {
        return ppd1->getCpuUsage() > ppd2->getCpuUsage();
    });

    // Update the strings to be drawn
    m_procCPUListData.clear();
    for (const auto& ppd : m_allProcessData) {

        m_procCPUListData.emplace_back(ppd->getName(), ppd->getCpuUsage());

        if (m_procCPUListData.size() >= m_numProcessesToDisplay) {
            break;
        }
    }
}

void ProcessMeasure::fillRAMData() {
    // Now sort the list in terms of memory usage and build strings for that
    std::sort(m_allProcessData.begin(), m_allProcessData.end(),
    [](const auto& ppd1, const auto& ppd2) {
        return ppd1->getWorkingSetSizeMB() > ppd2->getWorkingSetSizeMB();
    });

    m_procRAMListData.clear();
    for (const auto& ppd : m_allProcessData) {
        m_procRAMListData.emplace_back(ppd->getName(), ppd->getWorkingSetSizeMB());

        if (m_procRAMListData.size() >= m_numProcessesToDisplay) {
            break;
        }
    }
}

double ProcessMeasure::calculateCPUUsage(HANDLE pHandle, ProcessData& oldData) {
        // Find delta in the process's total CPU usage time
        FILETIME cTime;
        FILETIME eTime;
        FILETIME kTime;
        FILETIME uTime;
        GetProcessTimes(pHandle, &cTime, &eTime, &uTime, &kTime);
        const auto procKernelDiff{ subtractTimes(kTime, oldData.getKernelTime()) };
        const auto procUserDiff{ subtractTimes(uTime, oldData.getUserTime()) };
        const auto totalProc{ procKernelDiff + procUserDiff };

        // Find delta in the entire system's CPU usage time
        FILETIME sysIdle, sysKernel, sysUser;
        GetSystemTimes(&sysIdle, &sysKernel, &sysUser);
        const auto sysKernelDiff{ subtractTimes(sysKernel, oldData.getLastSystemKernelTime()) };
        const auto sysUserDiff{ subtractTimes(sysUser, oldData.getLastSystemUserTime()) };
        const auto totalSys{ sysKernelDiff + sysUserDiff };

        // Get the CPU usage as a percentage
        const double cpuUse = static_cast<double>(100 * totalProc) / static_cast<double>(totalSys);


        oldData.setTimes(cTime, eTime, kTime, uTime);

        return cpuUse;
}

void ProcessMeasure::updateProcList() {

    // Get the system snapshot of processes handle
    HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (processSnapshot == INVALID_HANDLE_VALUE) {
        fatalMessageBox("Failed to get process snapshot.");
        exit(1);
    }

    // Get the first process from the snapshot
    PROCESSENTRY32 pe{};
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(processSnapshot, &pe)) {
        fatalMessageBox("Failed to get first process from snapshot.");
        CloseHandle(processSnapshot);
        exit(1);
    }

    CloseHandle(processSnapshot);
}


void ProcessMeasure::populateList() {
    m_allProcessData.clear();

    // Get the process snapshot
    HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (processSnapshot == INVALID_HANDLE_VALUE) {
        fatalMessageBox("Failed to get process snapshot.");
    }

    // Get the first process from the snapshot
    PROCESSENTRY32 pe{};
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(processSnapshot, &pe)) {
        CloseHandle(processSnapshot);
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
        m_allProcessData.emplace_back(std::make_shared<ProcessData>(pHandle, pe.th32ProcessID, pe.szExeFile));

    } while (Process32Next(processSnapshot, &pe));

    CloseHandle(processSnapshot);
}

}