#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <Windows.h>
#include <GL/glew.h>

// TODO just forward declare the widgets
#include "FontManager.h"
#include "TimeWidget.h"
#include "HDDWidget.h"
#include "CPUStatsWidget.h"
#include "ProcessWidget.h"
#include "GraphWidget.h"
#include "SystemStatsWidget.h"
#include "MainWidget.h"
#include "MusicWidget.h"
#include "drawUtils.h"

namespace rg {

class CPUMeasure;
class GPUMeasure;
class RAMMeasure;
class NetMeasure;
class ProcessMeasure;
class DriveMeasure;
class DriveInfo;
class SystemMeasure;
class GLShaderHandler;
class UserSettings;

/* In charge of rendering each component's data to the window */
class Renderer {
public:
    Renderer() = default;
    ~Renderer() noexcept = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void init(HWND hWnd, uint32_t windowWidth, uint32_t windowHeight,
              const CPUMeasure& _cpu, const GPUMeasure& _gpu,
              const RAMMeasure& _ram, const NetMeasure& _net,
              const ProcessMeasure& _proc, const DriveMeasure& _drive,
              const MusicMeasure& _music, const SystemMeasure& _sys,
              const UserSettings& settings);

    /* Releases all resources */
    void release();

    /* Draws scene to the window */
    void draw(uint32_t ticks) const;

    void updateWindowSize(int32_t newWidth, int32_t newHeight);
private:
    void setViewports(int32_t windowWidth, int32_t windowHeight);

    /* Fill VBOs with intial vertex data */
    void initVBOs();
    /* Compiles and retrieves uniform locations */
    void initShaders();

    void initWidgets(const UserSettings& settings,
                     int32_t windowWidth, int32_t windowHeight,
                     const CPUMeasure& _cpu, const GPUMeasure& _gpu,
                     const RAMMeasure& _ram, const NetMeasure& _net,
                     const ProcessMeasure& _proc, const DriveMeasure& _drive,
                     const MusicMeasure& _music, const SystemMeasure& _sys);

    HWND m_renderTargetHandle{ nullptr };

    FontManager m_fontManager;

    TimeWidget m_timeWidget;
    HDDWidget m_hddWidget;
    CPUStatsWidget m_cpuStatsWidget;
    ProcessWidget m_processWidget;
    GraphWidget m_graphWidget;
    SystemStatsWidget m_systemStatsWidget;
    MainWidget m_mainWidget;
    MusicWidget m_musicWidget;

    // VBO members
    GLuint m_graphGridVertsID{ 0U };
    GLuint m_graphGridIndicesID{ 0U };
    GLsizei m_graphIndicesSize{ 0U };

    // Shaders
    GLuint m_cpuGraphShader{ 0U };

    // Uniform location (UL) variables
    GLint m_graphAlphaLoc{ -1 };
};

}
