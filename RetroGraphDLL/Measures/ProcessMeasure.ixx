export module RG.Measures:ProcessMeasure;

import :Measure;

import ConfigRefreshedEvent;

import RG.Measures.Data;

import std.core;

import "WindowsHeaderUnit.h";

namespace rg {

/* Tracks system processes and their CPU/RAM usage */
export class ProcessMeasure : public Measure {
public:
    ProcessMeasure();
    ~ProcessMeasure() noexcept;

    size_t getNumProcessesRunning() const { return m_allProcessData.size(); }

    int getPIDFromName(std::string_view name) const;

    /* Gets vector containing top CPU using processes and their CPU usage */
    const std::vector<std::pair<std::string, double>>& getProcCPUData() const { return m_procCPUListData; }

    /* Gets vector containing top RAM using processes and their RAM usage */
    const std::vector<std::pair<std::string, size_t>>& getProcRAMData() const { return m_procRAMListData; }

protected:
    /* Updates the currently tracked processes and their CPU usage.
       Stops tracking any processes that have exited */
    bool updateInternal() override;

private:
    /* Sets the debug privileges of the programs to allow reading of system processes */
    bool setDebugPrivileges(HANDLE hToken, LPCTSTR Privilege, bool enablePrivilege);

    /* Fills the CPU usage process vector with top CPU using processes */
    void fillCPUData();

    /* Fills the RAM usage process vector with top RAM using processes */
    void fillRAMData();

    /* Calculates the CPU usage of the given process */
    double calculateCPUUsage(HANDLE pHandle, ProcessData& oldData);

    /* Fills m_allProcessData with new process information */
    void populateList();

    /* Polls window's process list to find any new processes and adds their process data to the list */
    void detectNewProcesses();

    std::vector<std::unique_ptr<ProcessData>> m_allProcessData;

    unsigned int m_numCPUProcessesToDisplay{ 10U };
    unsigned int m_numRAMProcessesToDisplay{ 10U };
    std::vector<std::pair<std::string, double>> m_procCPUListData{ m_numCPUProcessesToDisplay };
    std::vector<std::pair<std::string, size_t>> m_procRAMListData{ m_numRAMProcessesToDisplay };
    ConfigRefreshedEvent::Handle m_configRefreshedHandle;
};

} // namespace rg
