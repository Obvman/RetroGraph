#pragma once

#include <stdint.h>
#include <Windows.h>
#include <GL/glew.h>

#include "FontManager.h"
#include "TimeWidget.h"
#include "HDDWidget.h"
#include "CPUStatsWidget.h"
#include "ProcessCPUWidget.h"
#include "ProcessRAMWidget.h"
#include "GraphWidget.h"
#include "SystemStatsWidget.h"
#include "MainWidget.h"
#include "MusicWidget.h"

namespace rg {

class RetroGraph;
class Window;
class CPUMeasure;
class GPUMeasure;
class RAMMeasure;
class NetMeasure;
class ProcessCPUMeasure;
class ProcessRAMMeasure;
class DriveMeasure;
class DriveInfo;
class SystemMeasure;
class GLShaderHandler;
class UserSettings;

/* In charge of rendering each component's data to the window */
class Renderer {
public:
    Renderer(const Window& w, const RetroGraph& _rg, 
             const UserSettings& settings);
    ~Renderer() noexcept;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    /* Draws scene to the window */
    void draw(uint32_t ticks) const;

    void updateWindowSize(int32_t newWidth, int32_t newHeight);

    void needsRedraw() const { m_systemStatsWidget.needsRedraw(); }
private:
    void setViewports(int32_t windowWidth, int32_t windowHeight);

    /* Fill VBOs with intial vertex data */
    void initVBOs();
    /* Compiles and retrieves uniform locations */
    void initShaders();

    HWND m_renderTargetHandle{ nullptr };

    const UserSettings* m_settings;
    FontManager m_fontManager;

    TimeWidget m_timeWidget;
    HDDWidget m_hddWidget;
    CPUStatsWidget m_cpuStatsWidget;
    ProcessRAMWidget m_processRAMWidget;
    ProcessCPUWidget m_processCPUWidget;
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
