#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <Windows.h>
#include <GL/glew.h>

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

    void init(HWND hWnd, uint16_t windowWidth, uint16_t windowHeight,
              const CPUMeasure& _cpu, const GPUMeasure& _gpu,
              const RAMMeasure& _ram, const NetMeasure& _net,
              const ProcessMeasure& _proc, const DriveMeasure& _drive,
              const SystemInfo& _sys);

    /* Releases all resources */
    void release();

    /* Draws scene to the window */
    void draw(uint32_t ticks) const;
private:
    void initViewportBuffers(uint16_t windowWidth, uint16_t windowHeight);
    /* Creates font data and loads draw data into OpenGL call lists */
    void initFonts(HWND hWnd);
    /* Fill VBOs with intial vertex data */
    void initVBOs();
    /* Compiles and retrieves uniform locations */
    void initShaders();

    void drawGraphWidget() const;
    void drawCpuGraph() const;
    void drawRamGraph() const;
    void drawGpuGraph() const;
    void drawNetGraph() const;

    void drawProcessWidget() const;
    void drawProcCPUList() const;
    void drawProcRAMList() const;
    void drawStatsWidget() const;

    void drawMainWidget() const;
    void drawCoreGraphs() const;

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

    std::vector<std::string> m_statsStrings;

    // Viewports for each widget
    GLint m_timeWidgetVP[4];
    GLint m_hddWidgetVP[4];
    GLint m_procWidgetVP[4];
    GLint m_statsWidgetVP[4];

    // Graph widget viewport and sub-viewports
    GLint m_graphWidgetVP[4]; // Viewport of all graphs together
    GLint m_cpuGraphVP[4]; // viewport of graph relative to graphWidgetViewport
    GLint m_ramGraphVP[4];
    GLint m_gpuGraphVP[4];
    GLint m_netGraphVP[4];

    // Main widget viewport and sub-viewports
    GLint m_mainWidgetVP[4];
    GLint m_coreGraphsVP[4];

    // Font members
    GLint stdFontBase; // The default font
    GLint stdFontBoldBase;
    GLint smlFontBase;
    GLint lrgFontBase;
    GLint timeFontBase;

    // VBO members
    GLuint m_graphGridVertsID;
    GLuint m_graphGridIndicesID;
    GLsizei m_graphIndicesSize;

    GLuint m_graphLineVertsID;
    GLuint m_graphLineIndicesID;
    GLsizei m_graphLineIndicesSize;

    // Shaders
    GLuint m_cpuGraphShader;

    // Uniform location (UL) variables
    GLint m_graphAlphaLoc;
};

}