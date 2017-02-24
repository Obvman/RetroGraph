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
    m_creationTime{},
    m_exitTime{},
    m_kernelTime{},
    m_userTime{},
    m_procName{} {

    FILETIME sysIdle;
    GetSystemTimes(&sysIdle, &m_lastSystemKernelTime, &m_lastSystemUserTime);
    //m_lastSystemCPUTime = { ftToULI(sysKernel).QuadPart + ftToULI(sysUser).QuadPart };

    // TODO get the timestamp of this object's creation (not the process)
    //m_lastSystemCPUTime = sysKernel.

    // Get memory information for the process
    /*PROCESS_MEMORY_COUNTERS memCounters;
    if (!GetProcessMemoryInfo(m_pHandle, &memCounters, sizeof(PROCESS_MEMORY_COUNTERS))) {
        fatalMessageBox("Failed to get process memory information.");
    }*/

    // Get CPU time information
    if (!GetProcessTimes(m_pHandle, &m_creationTime, &m_exitTime, &m_kernelTime, &m_userTime)) {
        fatalMessageBox("Failed to get process times.");
    }

    // TODO get process name without full path
    char procName[128];
    GetModuleFileNameEx(m_pHandle, nullptr, procName, 128);
    m_procName = std::string{ procName };

}

ProcessData::~ProcessData() {
    std::cout << "ProcessData destructor called\n";
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