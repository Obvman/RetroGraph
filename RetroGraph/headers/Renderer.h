#pragma once

#include <stdint.h>
#include <Windows.h>
#include <GL/glew.h>

namespace rg {

class CPUMeasure;
class GPUMeasure;
class RAMMeasure;
class ProcessMeasure;
class DriveMeasure;
class DriveInfo;
class SystemInfo;
class GLShaderHandler;

/* In charge of rendering each component's data to the window */
class Renderer {
public:
    Renderer(const CPUMeasure& _cpu, const GPUMeasure& _gpu, const RAMMeasure& _ram,
             const ProcessMeasure& _proc, const DriveMeasure& _drive, const SystemInfo& _sys,
             uint16_t windowWidth, uint16_t windowHeight);
    ~Renderer();

    void init(HWND hWnd);
    void initFonts(HWND hWnd);
    void initVBOs();
    void release();

    void draw(const GLShaderHandler& shaders) const;
private:
    void drawGraphWidget() const;
    void drawCpuGraph() const;
    void drawRamGraph() const;

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

    const CPUMeasure& m_cpuMeasure;
    const GPUMeasure& m_gpuMeasure;
    const RAMMeasure& m_ramMeasure;
    const ProcessMeasure& m_processMeasure;
    const DriveMeasure& m_driveMeasure;
    const SystemInfo& m_sysInfo;

    const GLint m_timeWidgetViewport[4];
    const GLint m_hddWidgetViewport[4];
    const GLint m_procWidgetViewport[4];
    const GLint m_statsWidgetViewport[4];

    const GLint m_graphWidgetViewport[4]; // Viewport of all graphs
    const GLint m_cpuGraphViewport[4]; // viewport of graph relative to graphWidgetViewport
    const GLint m_ramGraphViewport[4]; // viewport of graph relative to graphWidgetViewport

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
};

}