#pragma once

#include <memory>
#include <vector>
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

#include "ProcessData.h"

namespace rg {

class ProcessMeasure {
public:
    ProcessMeasure();
    ~ProcessMeasure();

    void init();
    void update(uint32_t ticks);
    void updateProcList();
    void draw() const;
private:
    uint64_t subtractTimes(const FILETIME& ftA, const FILETIME& ftB);
    double calculateCPUUsage(HANDLE pHandle, ProcessData& pd);
    void populateList();

    HANDLE m_processSnapshot;
    std::vector<std::unique_ptr<ProcessData>> m_allProcessData;
};

}