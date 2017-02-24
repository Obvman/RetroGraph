#pragma once
#include <Windows.h>
#include <Psapi.h>
#include <stdint.h>
#include <string>

namespace rg {

class ProcessData {
public:
    ProcessData(HANDLE pHandle, DWORD pID, const char* name);
    ~ProcessData();

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

    const FILETIME& getLastSystemKernelTime() const { return m_lastSystemKernelTime; }
    /* Returns FILETIME struct containing the total system user cpu time at
       the point this object was last updated */

    const FILETIME& getLastSystemUserTime() const { return m_lastSystemUserTime; }

    /* Returns the CPU usage as a percentage of this processes over the
       period between the previous and latest update of this object */
    double getCpuUsage() const { return m_cpuUsage; }

    void setTimes(const FILETIME& cTime, const FILETIME& eTime,
                  const FILETIME& kTime, const FILETIME& uTime);

    void setCpuUsage(double u) { m_cpuUsage = u; }

private:
    HANDLE m_pHandle;
    DWORD m_processID;
    std::string m_procName;

    FILETIME m_creationTime;
    FILETIME m_exitTime;
    FILETIME m_kernelTime;
    FILETIME m_userTime;
    FILETIME m_lastSystemKernelTime;
    FILETIME m_lastSystemUserTime;

    double m_cpuUsage;
};

}
