#pragma once
#include <Windows.h>
#include <Psapi.h>
#include <stdint.h>
#include <string>

namespace rg {

class ProcessData {
public:
    ProcessData(HANDLE pHandle, DWORD pID);
    ~ProcessData();

    DWORD getPID() const { return m_processID; }
    const std::string& getName() const { return m_procName; }

    const FILETIME& getCreationTime() const { return m_creationTime; }
    const FILETIME& getExitTime() const { return m_exitTime; }
    const FILETIME& getKernelTime() const { return m_kernelTime; }
    const FILETIME& getUserTime() const { return m_userTime; }

    void setCreationTime(const FILETIME& ft) { m_creationTime = ft; }
    void setExitTime(const FILETIME& ft) { m_exitTime = ft; }
    void setKernelTime(const FILETIME& ft) { m_kernelTime = ft; }
    void setUserTime(const FILETIME& ft) { m_userTime = ft; }

    uint64_t getLastSystemCPUTime() const { return m_lastSystemCPUTime; }
    FILETIME getLastSystemKernelTime() const { return m_lastSystemKernelTime; }
    FILETIME getLastSystemUserTime() const { return m_lastSystemUserTime; }

    void setTimes(const FILETIME& cTime, const FILETIME& eTime,
                  const FILETIME& kTime, const FILETIME& uTime);

private:

    HANDLE m_pHandle;
    DWORD m_processID;
    FILETIME m_creationTime;
    FILETIME m_exitTime;
    FILETIME m_kernelTime;
    FILETIME m_userTime;
    std::string m_procName;

    uint64_t m_lastSystemCPUTime;

    FILETIME m_lastSystemKernelTime;
    FILETIME m_lastSystemUserTime;
};

}
