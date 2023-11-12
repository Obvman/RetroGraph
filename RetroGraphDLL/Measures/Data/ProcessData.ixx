module;

#include "RGAssert.h"

export module ProcessData;

import Units;
import Utils;

import std.core;

import "WindowsHeaders.h";

#pragma comment(lib, "Ntdll.lib")

namespace rg {

export class ProcessData;

/* Storage class that contains various information about a system process */
class ProcessData {
public:
    ProcessData(HANDLE pHandle, DWORD pID, const char* name);
    ~ProcessData() noexcept;
    ProcessData(const ProcessData&) = delete;
    ProcessData& operator=(const ProcessData&) = delete;
    ProcessData(ProcessData&&) = delete;
    ProcessData& operator=(ProcessData&&) = delete;

    /* Gets the process ID */
    DWORD getPID() const { return m_processID; }

    /* Gets string of the processes executable name without the full path */
    const std::string& getName() const { return m_procName; }

    /* Returns FILETIME struct containing the time the processes was created */
    const FILETIME& getCreationTime() const { return m_creationTime; }

    /* Returns FILETIME struct containing the time the process exited */
    const FILETIME& getExitTime() const { return m_exitTime; }

    /* Returns FILETIME struct containing the CPU time the process has spent
       in the kernel mode */
    const FILETIME& getKernelTime() const { return m_kernelTime; }

    /* Returns FILETIME struct containing the CPU time the process has spent
       in the user mode */
    const FILETIME& getUserTime() const { return m_userTime; }

    /* Returns FILETIME struct containing the total system kernel cpu time at
       the point this object was last updated */
    const FILETIME& getLastSystemKernelTime() const {
        return m_lastSystemKernelTime;
    }

    /* Returns FILETIME struct containing the total system user cpu time at
       the point this object was last updated */
    const FILETIME& getLastSystemUserTime() const {
        return m_lastSystemUserTime;
    }

    /* Returns the current memory usage of the process */
    SIZE_T getWorkingSetSizeMB() const {
        return m_memCounters.WorkingSetSize / MB;
    }

    /* Returns the CPU usage as a percentage of this processes over the
       period between the previous and latest update of this object */
    double getCpuUsage() const { return m_cpuUsage; }

    void setTimes(const FILETIME& cTime, const FILETIME& eTime,
                  const FILETIME& kTime, const FILETIME& uTime);

    void setCpuUsage(double u) { m_cpuUsage = u; }

    /* Updates memory status struct with current process memory stats */
    void updateMemCounters();

private:
    HANDLE m_pHandle{ nullptr };
    DWORD m_processID{ 0 };
    std::string m_procName{ "" };
    PROCESS_MEMORY_COUNTERS m_memCounters{ };

    FILETIME m_creationTime{ };
    FILETIME m_exitTime{ };
    FILETIME m_kernelTime{ };
    FILETIME m_userTime{ };
    FILETIME m_lastSystemKernelTime{ };
    FILETIME m_lastSystemUserTime{ };

    double m_cpuUsage{ 0.0 };
};

ProcessData::ProcessData(HANDLE pHandle, DWORD pID, const char* name) :
    m_pHandle{ pHandle },
    m_processID{ pID },
    m_procName{ name } {

    // Remove the ".exe" extension from the process name
    const auto p{ m_procName.find(".exe") };
    if (p != std::string::npos) {
        m_procName.erase(p, 4);
    }
    // If the name is too long, truncate and add ellipses
    constexpr size_t cutoffLen{ 26U };
    if (m_procName.length() >= cutoffLen) {
        m_procName.erase(cutoffLen, m_procName.length() - cutoffLen);
        m_procName.append("...");
    }

    FILETIME sysIdle; // Dummy ft, won't be used
    GetSystemTimes(&sysIdle, &m_lastSystemKernelTime, &m_lastSystemUserTime);

    // Get memory information for the process
    updateMemCounters();

    // Get CPU time information for the process
    RGVERIFY(GetProcessTimes(m_pHandle, &m_creationTime, &m_exitTime, &m_kernelTime, &m_userTime),
             "Failed to get process times.");

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

    FILETIME sysIdle; // dummy
    GetSystemTimes(&sysIdle, &m_lastSystemKernelTime, &m_lastSystemUserTime);
}

void ProcessData::updateMemCounters() {
    RGVERIFY(GetProcessMemoryInfo(m_pHandle, &m_memCounters, sizeof(m_memCounters)),
             std::format("Failed to get process memory information. Error: ", GetLastError()).c_str());
}

}
