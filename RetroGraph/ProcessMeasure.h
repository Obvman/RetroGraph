#pragma once

#include <memory>
#include <vector>
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <GL/glew.h>

#include "ProcessData.h"

namespace rg {

class ProcessMeasure {
public:
    ProcessMeasure(GLint vpX, GLint vpY, GLint vpW, GLint vpH);
    ~ProcessMeasure();

    void init();
    void update(uint32_t ticks);
    void updateProcList();
    void draw() const;
private:
    void drawUsageList() const;
    double calculateCPUUsage(HANDLE pHandle, ProcessData& pd);
    void populateList();

    HANDLE m_processSnapshot;
    std::vector<std::unique_ptr<ProcessData>> m_allProcessData;

    // Rendering members
    GLint m_viewportStartX;
    GLint m_viewportStartY;
    GLint m_viewportWidth;
    GLint m_viewportHeight;
};

}