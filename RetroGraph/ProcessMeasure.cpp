#include "ProcessMeasure.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "ProcessData.h"
#include "utils.h"

namespace rg {

ProcessMeasure::ProcessMeasure() :
    m_processSnapshot{},
    m_allProcessData{} {
}


ProcessMeasure::~ProcessMeasure() {
}

void ProcessMeasure::init() {
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

    // Enumerate the process list
    DWORD processIDs[1024];
    DWORD bytesUsed;

    if (!EnumProcesses(processIDs, sizeof(processIDs), &bytesUsed)) {
        fatalMessageBox("Failed to Enumerate Processes");
    }

    // Check how many IDs were returned
    const auto numProcesses = DWORD{ bytesUsed / sizeof(DWORD) };

    // Loop over buffer of process IDs to get data and populate allProcessData vec
    for (auto i{ 0U }; i < numProcesses; ++i) {
        const auto pHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                         false, processIDs[i]);

        // Check if we've found a usable process handle
        if (!pHandle) {
            const auto error = GetLastError();
            // If access is denied or the process is the system idle process, just silently skip the process
            if (error != ERROR_ACCESS_DENIED && processIDs[i] != 0) {
                fatalMessageBox("Failed to open process. Code: " +
                                std::to_string(error));
            }

            continue;
        }

        // Populate the vector
        m_allProcessData.emplace_back(std::make_unique<ProcessData>(pHandle, processIDs[i]));
    }

    CloseHandle(m_processSnapshot);
}

// Algorithm:
// init():
// Fill vector with process objects (contain ID, user/kernel clock times, timestamp of object creation)
//
// update():
// for pd in vector:
//    procHandle = getProcess(pd.getID())
//    if procHandle == null:
//       remove pd from vector
//    else:
//       get new user/kernel clock times
//       calculate cpu usage since last round using saved time data
//
//  every 10 seconds, Query OS for list of processes to update

// TODO benchmark using vector vs list

void ProcessMeasure::update(uint32_t ticks) {
    // Update the process list vector every 10 seconds
    if (ticks % 100 == 0) {
        std::cout << "Updating process list\n";
        //updateProcList();
    }

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

            // Find delta in the process's total CPU usage time
            FILETIME cTime;
            FILETIME eTime;
            FILETIME kTime;
            FILETIME uTime;
            GetProcessTimes(pHandle, &cTime, &eTime, &uTime, &kTime);
            const auto procKernelDiff{ SubtractTimes(kTime, pd.getKernelTime()) };
            const auto procUserDiff{ SubtractTimes(uTime, pd.getUserTime()) };
            const auto totalProc{ procKernelDiff + procUserDiff };

            // Find delta in the entire system's CPU usage time
            FILETIME sysIdle, sysKernel, sysUser;
            GetSystemTimes(&sysIdle, &sysKernel, &sysUser);
            const auto sysKernelDiff{ SubtractTimes(sysKernel, pd.getLastSystemKernelTime()) };
            const auto sysUserDiff{ SubtractTimes(sysUser, pd.getLastSystemUserTime()) };
            const auto totalSys{ sysKernelDiff + sysUserDiff };

            // Get the CPU usage as a percentage
            const double cpuUse = static_cast<double>(100 * totalProc) / static_cast<double>(totalSys);

            if (pd.getName() == "D:\\ProgramFiles\\VLC\\vlc.exe") {
                std::cout << "VLC CPU Usage: " << cpuUse << '\n';
            }

            //std::cout << std::setprecision(4) << pd.getName() << ": " << cpuUse << "%\n";

            pd.setTimes(cTime, eTime, kTime, uTime);
            ++it;
        }
    }
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
    enumerate();

    CloseHandle(m_processSnapshot);
}

void ProcessMeasure::enumerate() {
    // Get list of process IDs
    DWORD processIDs[1024];
    DWORD cbNeeded;

    if (!EnumProcesses(processIDs, sizeof(processIDs), &cbNeeded)) {
        fatalMessageBox("Failed to Enumerate Processes");
    }

    // Check how many IDs were returned
    auto numProcesses = DWORD{ cbNeeded / sizeof(DWORD) };

    for (auto i{ 0U }; i < numProcesses; ++i) {
        auto pHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                   false, processIDs[i]);
        if (!pHandle) {
            auto error = GetLastError();
            // If access is denied or the process is the system idle process, just silently skip the process
            if (error != ERROR_ACCESS_DENIED && processIDs[i] != 0) {
                fatalMessageBox("Failed to open process. Code: " + std::to_string(error));
            }
            continue;
        }

        // TODO check if the process is already stored before adding it to the vector
        m_allProcessData.emplace_back(std::make_unique<ProcessData>(pHandle, processIDs[i]));

        FILETIME creationFT;
        FILETIME kernelFT;
        FILETIME userFT;
        FILETIME exitFT;
        if (!GetProcessTimes(pHandle, &creationFT, &exitFT, &kernelFT, &userFT)) {
            fatalMessageBox("Failed to get process times.");
        }
        auto creationTime = (static_cast<uint64_t>(creationFT.dwHighDateTime) << 32) + creationFT.dwLowDateTime;
        auto kernelTime = (static_cast<uint64_t>(kernelFT.dwHighDateTime) << 32) + kernelFT.dwLowDateTime;
        auto userTime = (static_cast<uint64_t>(userFT.dwHighDateTime) << 32) + userFT.dwLowDateTime;
        auto exitTime = (static_cast<uint64_t>(exitFT.dwHighDateTime) << 32) + exitFT.dwLowDateTime;
        auto total = kernelTime + userTime;

    }

}

void ProcessMeasure::slowLoop(PROCESSENTRY32* pe) {
    // Loop through each process in the snapshot, recording relevant information
    do {
        auto pHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pe->th32ProcessID);
        if (!pHandle) {
            // Couldn't get the process handle, do nothing for now
        } else {
            auto priorityClass = GetPriorityClass(pHandle);
            if (!priorityClass) {
                fatalMessageBox("Failed to retrieve priority class.");
            }
        }

        std::cout << "Name: " << pe->szExeFile << '\n';

        // Get memory information for the process
        PROCESS_MEMORY_COUNTERS memCounters;
        if (GetProcessMemoryInfo(pHandle, &memCounters, sizeof(PROCESS_MEMORY_COUNTERS))) {
            //std::cout << "Working set: " << memCounters.WorkingSetSize / (1024.0f * 1024.0f * 1024.0f) << "GB\n";
        }

        // Get CPU time information
        FILETIME creationTime;
        FILETIME exitTime;
        FILETIME kernelTime;
        FILETIME userTime;
        if (GetProcessTimes(pHandle, &creationTime, &exitTime, &kernelTime, &userTime)) {
            SYSTEMTIME creationSysTime;
            FileTimeToSystemTime(&creationTime, &creationSysTime);
        }

        CloseHandle(pHandle);
    } while (Process32Next(m_processSnapshot, pe));
}

uint64_t ProcessMeasure::SubtractTimes(const FILETIME& ftA, const FILETIME& ftB) {
     LARGE_INTEGER a, b;
     a.LowPart = ftA.dwLowDateTime;
     a.HighPart = ftA.dwHighDateTime;

     b.LowPart = ftB.dwLowDateTime;
     b.HighPart = ftB.dwHighDateTime;

     return a.QuadPart - b.QuadPart;
}


}