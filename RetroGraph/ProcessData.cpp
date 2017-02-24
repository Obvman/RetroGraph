#include "ProcessData.h"
#include <iostream>

#include "utils.h"

namespace rg {

constexpr uint64_t ftSecond{ 10000000 };
constexpr uint64_t ftMinute{ 60 * ftSecond };
constexpr uint64_t ftHour{ 60 * ftMinute };
constexpr uint64_t ftDay{ 24 * ftHour };


ProcessData::ProcessData(HANDLE pHandle, DWORD pID, const char* name) :
    m_pHandle{ pHandle },
    m_processID{ pID },
    m_procName{ name },
    m_creationTime{},
    m_exitTime{},
    m_kernelTime{},
    m_userTime{},
    m_cpuUsage{ 0.0 } {

    FILETIME sysIdle; // Dummy ft, won't be used
    GetSystemTimes(&sysIdle, &m_lastSystemKernelTime, &m_lastSystemUserTime);

    // Get memory information for the process
    /*PROCESS_MEMORY_COUNTERS memCounters;
    if (!GetProcessMemoryInfo(m_pHandle, &memCounters, sizeof(PROCESS_MEMORY_COUNTERS))) {
        fatalMessageBox("Failed to get process memory information.");
    }*/

    // Get CPU time information
    if (!GetProcessTimes(m_pHandle, &m_creationTime, &m_exitTime, &m_kernelTime, &m_userTime)) {
        fatalMessageBox("Failed to get process times.");
    }

}

ProcessData::~ProcessData() {
    CloseHandle(m_pHandle);
}

void ProcessData::setTimes(const FILETIME& cTime, const FILETIME& eTime,
                           const FILETIME& kTime, const FILETIME& uTime) {
    m_creationTime = cTime;
    m_exitTime = eTime;
    m_kernelTime = kTime;
    m_userTime = uTime;

    FILETIME sysIdle;
    GetSystemTimes(&sysIdle, &m_lastSystemKernelTime, &m_lastSystemUserTime);
}


}