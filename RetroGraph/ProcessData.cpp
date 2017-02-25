#include "ProcessData.h"
namespace rg {


ProcessData::ProcessData(HANDLE pHandle, DWORD pID, const char* name) :
    m_pHandle{ pHandle },
    m_processID{ pID },
    m_procName{ name },
    m_memCounters{},
    m_creationTime{},
    m_exitTime{},
    m_kernelTime{},
    m_userTime{},
    m_cpuUsage{ 0.0 } {

    FILETIME sysIdle; // Dummy ft, won't be used
    GetSystemTimes(&sysIdle, &m_lastSystemKernelTime, &m_lastSystemUserTime);

    // Get memory information for the process
    updateMemCounters();

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

void ProcessData::updateMemCounters() {
    if (!GetProcessMemoryInfo(m_pHandle, &m_memCounters, sizeof(decltype(m_memCounters)))) {
        fatalMessageBox("Failed to get process memory information.");
    }
}

}