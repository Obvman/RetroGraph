#pragma once

#include <memory>
#include <vector>
#include <utility>
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <GL/glew.h>

#include "ProcessData.h"

namespace rg {

/* Tracks system processes and their CPU/RAM usage */
class ProcessMeasure {
public:
    ProcessMeasure();
    ~ProcessMeasure();

    /* Initialises the measure with system processes information */
    void init();

    /* Updates the currently tracked processes and their CPU usage.
       Stops tracking any processes that have exited */
    void update(uint32_t ticks);

    /* Gets vector containing top CPU using processes and their CPU usage */
    const std::vector<std::pair<std::string, double>>& getProcCPUData() const { return m_procCPUListData; }

    /* Gets vector containing top RAM using processes and their RAM usage */
    const std::vector<std::pair<std::string, size_t>>& getProcRAMData() const { return m_procRAMListData; }
private:
    /* Refreshes the list of tracked processes to begin tracking any
       newly created processes */
    void updateProcList();

    /* Fills the CPU usage process vector with top CPU using processes */
    void fillCPUData();

    /* Fills the RAM usage process vector with top RAM using processes */
    void fillRAMData();

    /* Calculates the CPU usage of the given process */
    double calculateCPUUsage(HANDLE pHandle, ProcessData& oldData);

    /* Fills m_allProcessData with new process information */
    void populateList();

    std::vector<std::shared_ptr<ProcessData>> m_allProcessData;

    uint16_t m_numProcessesToDisplay{ 7 };
    std::vector<std::pair<std::string, double>> m_procCPUListData;
    std::vector<std::pair<std::string, size_t>> m_procRAMListData;

};

}