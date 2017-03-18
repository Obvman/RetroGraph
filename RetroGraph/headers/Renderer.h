#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <Windows.h>
#include <GL/glew.h>

#include "FontManager.h"

namespace rg {

constexpr size_t VP_X{ 0U };
constexpr size_t VP_Y{ 1U };
constexpr size_t VP_WIDTH{ 2U };
constexpr size_t VP_HEIGHT{ 3U };


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
    /* Fills all the viewport arrays with hard-coded values */
    void initViewportBuffers(uint32_t windowWidth, uint32_t windowHeight);
    /* Fill VBOs with intial vertex data */
    void initVBOs();
    /* Compiles and retrieves uniform locations */
    void initShaders();

    // Widgets
    void drawMainWidget() const;

    void drawRightGraphWidget() const;
    void drawCoreGraphs() const;

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
    GLint m_timeWidgetVP[4];
    GLint m_hddWidgetVP[4];
    GLint m_procWidgetVP[4];
    GLint m_statsWidgetVP[4];

    // Graph widget viewport and sub-viewports
    GLint m_leftGraphWidgetVP[4]; // Viewport of all graphs together
    GLint m_cpuGraphVP[4]; // viewport of graph relative to graphWidgetViewport
    GLint m_ramGraphVP[4];
    GLint m_gpuGraphVP[4];
    GLint m_netGraphVP[4];

    // Main widget viewport and sub-viewports
    GLint m_mainWidgetVP[4];
    GLint m_rightGraphWidgetVP[4]; // Viewport of all graphs together
    GLint m_coreGraphsVP[4];

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