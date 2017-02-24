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

    DWORD getPID() const { return m_processID; }
    const std::string& getName() const { return m_procName; }

    const FILETIME& getCreationTime() const { return m_creationTime; }
    const FILETIME& getExitTime() const { return m_exitTime; }
    const FILETIME& getKernelTime() const { return m_kernelTime; }
    const FILETIME& getUserTime() const { return m_userTime; }

    FILETIME getLastSystemKernelTime() const { return m_lastSystemKernelTime; }
    FILETIME getLastSystemUserTime() const { return m_lastSystemUserTime; }

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
