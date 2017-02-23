#pragma once
#include <Windows.h>
#include <Psapi.h>
#include <stdint.h>

namespace rg {

class ProcessData {
public:
    ProcessData(HANDLE pHandle, DWORD pID);
    ~ProcessData();

    void update();
    uint64_t getCPUUsagePercent() const;
    uint64_t getCycles() const { return m_cycles; }
    DWORD getPID() const { return m_processID; }

private:
    HANDLE m_pHandle;
    DWORD m_processID;
    PROCESS_MEMORY_COUNTERS m_memCounters;
    uint64_t m_creationTime;
    uint64_t m_kernelTime;
    uint64_t m_userTime;
    uint64_t m_exitTime;
    uint64_t m_cycles;

    uint64_t m_lastCPUCycleCount;
};

}
