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

    /* Initialises the measure with system processes information */
    void init();

    /* Updates the currently tracked processes and their CPU usage.
       Stops tracking any processes that have exited */
    void update(uint32_t ticks);

    /* Refreshes the list of tracked processes to begin tracking any
       newly created processes */
    void updateProcList();

    /* Draws the measure's components */
    void draw() const;

private:
    /* Draws sorted list of top CPU usage processes */
    void drawCPUUsageList() const;

    /* Draws sorted list of top RAM usage processes */
    void drawRAMUsageList() const;

    void fillCPUStrings();
    void fillRAMStrings();

    /* Calculates the CPU usage of the given process */
    double calculateCPUUsage(HANDLE pHandle, ProcessData& pd);

    /* Fills m_allProcessData with new process information */
    void populateList();

    std::vector<std::unique_ptr<ProcessData>> m_allProcessData;

    uint16_t m_numProcessesToDisplay{ 7 };
    std::vector<std::string> m_processCPUDrawStrings{ m_numProcessesToDisplay };
    std::vector<std::string> m_processRAMDrawStrings{ m_numProcessesToDisplay };

    // Rendering members
    GLint m_viewportStartX;
    GLint m_viewportStartY;
    GLint m_viewportWidth;
    GLint m_viewportHeight;

};

}