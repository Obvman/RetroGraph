#include "ProcessMeasure.h"
#include <iostream>
#include <algorithm>

#include "ProcessData.h"
#include "utils.h"

namespace rg {

ProcessMeasure::ProcessMeasure() :
    m_processSnapshot{ CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0) },
    m_allProcessData{} {
}


ProcessMeasure::~ProcessMeasure() {
}

void ProcessMeasure::init() {
    //update();
}

void ProcessMeasure::update() {
    // Clear the list of processes, it's likely outdated
    m_allProcessData.clear();

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


        //auto totalCPUTime = (kernelTime + userTime) * 100;
        //std::cout << "sysTime: " << totalCPUTime << '\n';

        //static FILETIME prevIdle;
        //static FILETIME prevKernel;
        //static FILETIME prevUser;
        //GetSystemTimes(&idle, &kernel, &user);
        //auto idleTotal = (static_cast<uint64_t>(idle.dwHighDateTime) << 32) + idle.dwLowDateTime;
        //auto kernelTotal = (static_cast<uint64_t>(kernel.dwHighDateTime) << 32) + kernel.dwLowDateTime;
        //auto userTotal = (static_cast<uint64_t>(user.dwHighDateTime) << 32) + user.dwLowDateTime;

        // TODO get previous update cycles values for times and compare
    }



    // Sort the process data vector by CPU usage

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

}