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
class SystemInfo;
class GLShaderHandler;

/* In charge of rendering each component's data to the window */
class Renderer {
public:
    Renderer(const CPUMeasure& _cpu, const GPUMeasure& _gpu, const RAMMeasure& _ram,
             const ProcessMeasure& _proc, const DriveMeasure& _drive, const SystemInfo& _sys,
             uint16_t windowWidth, uint16_t windowHeight);
    ~Renderer();

    void initFonts(HWND hWnd);
    void release();

    void draw(const GLShaderHandler& shaders) const;
private:

    void drawTimeWidget() const;

    HWND m_renderTargetHandle;

    const CPUMeasure& m_cpuMeasure;
    const GPUMeasure& m_gpuMeasure;
    const RAMMeasure& m_ramMeasure;
    const ProcessMeasure& m_processMeasure;
    const DriveMeasure& m_driveMeasure;
    const SystemInfo& m_sysInfo;

    const GLint m_timeWidgetViewport[4];

    GLint stdFontBase; // The default font
    GLint stdFontBoldBase;
    GLint lrgFontBase;
};

}