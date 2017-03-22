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
class SystemInfo;
class GLShaderHandler;
class UserSettings;

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
              const MusicMeasure& _music, const SystemInfo& _sys,
              const UserSettings& settings);

    /* Releases all resources */
    void release();

    /* Draws scene to the window */
    void draw(uint32_t ticks) const;
private:
    /* Fill VBOs with intial vertex data */
    void initVBOs();
    /* Compiles and retrieves uniform locations */
    void initShaders();

    void initWidgets(const UserSettings& settings,
                     int32_t windowWidth, int32_t windowHeight,
                     const CPUMeasure& _cpu, const GPUMeasure& _gpu,
                     const RAMMeasure& _ram, const NetMeasure& _net,
                     const ProcessMeasure& _proc, const DriveMeasure& _drive,
                     const MusicMeasure& _music, const SystemInfo& _sys);

    HWND m_renderTargetHandle;

    FontManager m_fontManager;

    // TODO refactor these into vector of Widget ptrs
    TimeWidget m_timeWidget;
    HDDWidget m_hddWidget;
    CPUStatsWidget m_cpuStatsWidget;
    ProcessWidget m_processWidget;
    GraphWidget m_graphWidget;
    SystemStatsWidget m_systemStatsWidget;
    MainWidget m_mainWidget;
    MusicWidget m_musicWidget;

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
