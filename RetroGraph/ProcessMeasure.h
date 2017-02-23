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
    void update();
private:
    void enumerate();
    void slowLoop(PROCESSENTRY32* pe);

    HANDLE m_processSnapshot;
    std::vector<std::unique_ptr<ProcessData>> m_allProcessData;
};

}