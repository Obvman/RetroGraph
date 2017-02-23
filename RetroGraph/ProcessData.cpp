#include "ProcessData.h"
#include <iostream>

#include "utils.h"

namespace rg {

constexpr uint64_t ftSecond{ 10000000 };
constexpr uint64_t ftMinute{ 60 * ftSecond };
constexpr uint64_t ftHour{ 60 * ftMinute };
constexpr uint64_t ftDay{ 24 * ftHour };


ProcessData::ProcessData(HANDLE pHandle, DWORD pID) :
    m_pHandle{ pHandle },
    m_processID{ pID },
    m_memCounters{},
    m_creationTime{},
    m_kernelTime{},
    m_userTime{},
    m_exitTime{},
    m_lastCPUCycleCount{ 0U } {

    // Get memory information for the process
    PROCESS_MEMORY_COUNTERS memCounters;
    if (!GetProcessMemoryInfo(m_pHandle, &memCounters, sizeof(PROCESS_MEMORY_COUNTERS))) {
        fatalMessageBox("Failed to get process memory information.");
    }

    FILETIME creationTime;
    FILETIME kernelTime;
    FILETIME userTime;
    FILETIME exitTime;

    // Get CPU time information
    if (!GetProcessTimes(m_pHandle, &creationTime, &exitTime, &kernelTime, &userTime)) {
        fatalMessageBox("Failed to get process times.");
    }

    // Copy time into 64-bit type
    m_creationTime = (static_cast<uint64_t>(creationTime.dwHighDateTime) << 32) + creationTime.dwLowDateTime;
    m_kernelTime = (static_cast<uint64_t>(kernelTime.dwHighDateTime) << 32) + kernelTime.dwLowDateTime;
    m_userTime = (static_cast<uint64_t>(userTime.dwHighDateTime) << 32) + userTime.dwLowDateTime;
    m_exitTime = (static_cast<uint64_t>(exitTime.dwHighDateTime) << 32) + exitTime.dwLowDateTime;

    const auto sysTime{ m_kernelTime + m_userTime };
    //std::cout << "SysTime: " << sysTime << '\n';

    QueryProcessCycleTime(m_pHandle, &m_cycles);
    //std::cout << "Process cycles: " << cycleTime << '\n';

}

void ProcessData::update() {
    // Get updated process handle

    //m_lastUpdate = getTimestamp();

    uint64_t cycles{ 0U };
    QueryProcessCycleTime(m_pHandle, &cycles);

    auto cpuDelta = cycles - m_lastCPUCycleCount;

    m_lastCPUCycleCount = cycles;
}

uint64_t ProcessData::getCPUUsagePercent() const {

    return 0;
}

ProcessData::~ProcessData() {
    CloseHandle(m_pHandle);
}

}