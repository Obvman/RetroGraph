#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <Windows.h>
#include <GL/glew.h>

#include "FontManager.h"
#include "drawUtils.h"

namespace rg {


class CPUMeasure;
class GPUMeasure;
class RAMMeasure;
class NetMeasure;
class ProcessMeasure;
class DriveMeasure;
class DriveInfo;
class SystemInfo;
class GLShaderHandler;

/* In charge of rendering each component's data to the window */
class Renderer {
public:
    Renderer();
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void init(HWND hWnd, uint32_t windowWidth, uint32_t windowHeight,
              const CPUMeasure& _cpu, const GPUMeasure& _gpu,
              const RAMMeasure& _ram, const NetMeasure& _net,
              const ProcessMeasure& _proc, const DriveMeasure& _drive,
              const SystemInfo& _sys);

    /* Releases all resources */
    void release();

    /* Draws scene to the window */
    void draw(uint32_t ticks) const;
private:
    /* Private Functions */

    /* Fills all the viewport arrays with hard-coded values */
    void initViewports(uint32_t windowWidth, uint32_t windowHeight);
    /* Fill VBOs with intial vertex data */
    void initVBOs();
    /* Compiles and retrieves uniform locations */
    void initShaders();

    // Widgets
    void drawMainWidget() const;

    void drawRightGraphWidget() const;
    void drawCoreGraphs() const;
    void drawRightStatsWidget() const;

    void drawLeftGraphWidget() const;
    void drawCpuGraph() const;
    void drawRamGraph() const;
    void drawGpuGraph() const;
    void drawNetGraph() const;

    void drawProcessWidget() const;
    void drawProcCPUList() const;
    void drawProcRAMList() const;
    void drawStatsWidget() const;

    /* Draw labelled capacity bars of all fixed drives in the system */
    void drawHDDWidget() const;

    /* Draws a labelled capacity bar for the given DriveInfo object */
    void drawHDDBar(const DriveInfo& di) const;

    /* Draws widget with time, date, uptime */
    void drawTimeWidget() const;


    HWND m_renderTargetHandle;

    // measure observers
    const CPUMeasure* m_cpuMeasure;
    const GPUMeasure* m_gpuMeasure;
    const RAMMeasure* m_ramMeasure;
    const NetMeasure* m_netMeasure;
    const ProcessMeasure* m_processMeasure;
    const DriveMeasure* m_driveMeasure;
    const SystemInfo* m_sysInfo;

    FontManager m_fontManager;

    std::vector<std::string> m_statsStrings;

    // Viewports for each widget
    Viewport m_timeVP;
    Viewport m_hddVP;
    Viewport m_procVP;
    Viewport m_statsVP;

    // Graph widget viewport and sub-viewports
    Viewport m_leftGraphWidgetVP; // Viewport of all graphs together
    Viewport m_cpuGraphVP; // viewport of graph relative to graphWidgetViewport
    Viewport m_ramGraphVP;
    Viewport m_gpuGraphVP;
    Viewport m_netGraphVP;

    // Main widget viewport and sub-viewports
    Viewport m_mainWidgetVP;
    Viewport m_rightGraphWidgetVP; // Viewport of all graphs together
    Viewport m_rightStatsVP;
    Viewport m_coreGraphsVP;

    // VBO members
    GLuint m_graphGridVertsID;
    GLuint m_graphGridIndicesID;
    GLsizei m_graphIndicesSize;

    // Shaders
    GLuint m_cpuGraphShader;

    // Uniform location (UL) variables
    GLint m_graphAlphaLoc;
};

}
