#pragma once

#include "stdafx.h"

#include <memory>
#include <vector>
#include <utility>
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <GL/glew.h>

#include "ProcessData.h"
#include "Measure.h"

namespace rg {

class UserSettings;

/* Tracks system processes and their CPU/RAM usage */
class ProcessMeasure : public Measure {
public:
    ProcessMeasure();
    ~ProcessMeasure() noexcept = default;
    ProcessMeasure(const ProcessMeasure&) = delete;
    ProcessMeasure& operator=(const ProcessMeasure&) = delete;
    ProcessMeasure(ProcessMeasure&&) = delete;
    ProcessMeasure& operator=(ProcessMeasure&&) = delete;

    /* Updates the currently tracked processes and their CPU usage.
       Stops tracking any processes that have exited */
    void update(uint32_t ticks) override;

    size_t getNumProcessesRunning() const { return m_allProcessData.size(); }

    int32_t getPIDFromName(const std::string& name) const;

    /* Gets vector containing top CPU using processes and their CPU usage */
    const std::vector<std::pair<std::string, double>>& getProcCPUData() const {
        return m_procCPUListData;
    }

    /* Gets vector containing top RAM using processes and their RAM usage */
    const std::vector<std::pair<std::string, size_t>>& getProcRAMData() const {
        return m_procRAMListData;
    }
private:
    bool shouldUpdate(uint32_t ticks) const override;

    /* Sets the debug privileges of the programs to allow reading of system
     * processes */
    bool setDebugPrivileges(HANDLE hToken, LPCTSTR Privilege,
                            bool enablePrivilege);

    /* Fills the CPU usage process vector with top CPU using processes */
    void fillCPUData();

    /* Fills the RAM usage process vector with top RAM using processes */
    void fillRAMData();

    /* Calculates the CPU usage of the given process */
    double calculateCPUUsage(HANDLE pHandle, ProcessData& oldData);

    /* Fills m_allProcessData with new process information */
    void populateList();

    /* Polls window's process list to find any new processes and adds their
       process data to the list */
    void detectNewProcesses();


    std::vector<std::shared_ptr<ProcessData>> m_allProcessData;

    uint32_t m_numCPUProcessesToDisplay{ 10U };
    uint32_t m_numRAMProcessesToDisplay{ 10U };
    std::vector<std::pair<std::string, double>> 
        m_procCPUListData{ m_numCPUProcessesToDisplay };
    std::vector<std::pair<std::string, size_t>> 
        m_procRAMListData{ m_numRAMProcessesToDisplay };
};

} // namespace rg
