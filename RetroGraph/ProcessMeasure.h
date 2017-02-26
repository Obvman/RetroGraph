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

class ProcessMeasure {
public:
    ProcessMeasure();
    ~ProcessMeasure();

    /* Initialises the measure with system processes information */
    void init();

    /* Updates the currently tracked processes and their CPU usage.
       Stops tracking any processes that have exited */
    void update(uint32_t ticks);


    const std::vector<std::pair<std::string, double>>& getProcCPUData() const { return m_procCPUListData; }
    const std::vector<std::pair<std::string, size_t>>& getProcRAMData() const { return m_procRAMListData; }

private:
    /* Refreshes the list of tracked processes to begin tracking any
       newly created processes */
    void updateProcList();

    void fillCPUData();
    void fillRAMData();

    /* Calculates the CPU usage of the given process */
    double calculateCPUUsage(HANDLE pHandle, ProcessData& pd);

    /* Fills m_allProcessData with new process information */
    void populateList();

    std::vector<std::unique_ptr<ProcessData>> m_allProcessData;

    uint16_t m_numProcessesToDisplay{ 7 };
    std::vector<std::pair<std::string, double>> m_procCPUListData;
    std::vector<std::pair<std::string, size_t>> m_procRAMListData;

};

}