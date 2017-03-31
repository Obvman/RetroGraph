#include "../headers/Renderer.h"

#include <sstream>
#include <iostream>

#include "../headers/CPUMeasure.h"
#include "../headers/GPUMeasure.h"
#include "../headers/RAMMeasure.h"
#include "../headers/NetMeasure.h"
#include "../headers/ProcessMeasure.h"
#include "../headers/DriveMeasure.h"
#include "../headers/SystemInfo.h"
#include "../headers/UserSettings.h"
#include "../headers/colors.h"
#include "../headers/utils.h"
#include "../headers/GLShaders.h"

namespace rg {

Renderer::Renderer() :
    m_renderTargetHandle{ nullptr },
    m_fontManager{}
{}

Renderer::~Renderer() {
}

void Renderer::init(HWND hWnd, uint32_t windowWidth, uint32_t windowHeight,
                    const CPUMeasure& _cpu, const GPUMeasure& _gpu,
                    const RAMMeasure& _ram, const NetMeasure& _net,
                    const ProcessMeasure& _proc, const DriveMeasure& _drive,
                    const MusicMeasure& _music, const SystemInfo& _sys,
                    const UserSettings& settings) {
    m_renderTargetHandle = hWnd;

    m_fontManager.init(hWnd, windowHeight);

    initWidgets(settings, windowWidth, windowHeight,
            _cpu, _gpu, _ram, _net, _proc, _drive, _music, _sys);

    initVBOs();
    initShaders();
}

void Renderer::draw(uint32_t) const {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, BGCOLOR_A);

    m_timeWidget.draw();
    m_hddWidget.draw();
    m_cpuStatsWidget.draw();
    m_processWidget.draw();
    m_graphWidget.draw();
    m_systemStatsWidget.draw();
    m_mainWidget.draw();
    m_musicWidget.draw();

}

void Renderer::release() {
    // Free VBO memory
    glDeleteBuffers(1, &m_graphGridVertsID);
    glDeleteBuffers(1, &m_graphGridIndicesID);

    m_fontManager.release();
}

void Renderer::updateWindowSize(int32_t newWidth, int32_t newHeight) {
    setViewports(newWidth, newHeight);

}

/********************* Private Functions ********************/

void Renderer::initWidgets(const UserSettings&,
                           int32_t windowWidth, int32_t windowHeight,
                           const CPUMeasure& _cpu, const GPUMeasure& _gpu,
                           const RAMMeasure& _ram, const NetMeasure& _net,
                           const ProcessMeasure& _proc, const DriveMeasure& _drive,
                           const MusicMeasure& _music, const SystemInfo& _sys) {
    const auto widgetW{ windowWidth/5 };
    const auto widgetH{ windowHeight/6 };
    const auto sideWidgetH{ windowHeight/2 };

    m_timeWidget.init(&m_fontManager, &_cpu, &_net);

    m_hddWidget.init(&m_fontManager, &_drive);

    m_cpuStatsWidget.init(&m_fontManager, &_cpu);

    m_processWidget.init(&m_fontManager, &_proc);

    m_graphWidget.init(&m_fontManager, &_cpu, &_ram, &_net, &_gpu);

    m_systemStatsWidget.init(&m_fontManager, &_sys, &_cpu, &_net);

    m_mainWidget.init(&m_fontManager);

    m_musicWidget.init(&m_fontManager, &_music);

    setViewports(windowWidth, windowHeight);
}

void Renderer::setViewports(int32_t windowWidth, int32_t windowHeight) {
    const auto widgetW{ windowWidth/5 };
    const auto widgetH{ windowHeight/6 };
    const auto sideWidgetH{ windowHeight/2 };

    m_timeWidget.setViewport(Viewport{ marginX, windowHeight - marginY -
            widgetH, widgetW, widgetH});

    m_hddWidget.setViewport(Viewport{windowWidth - widgetW - marginX,
            windowHeight - marginY - widgetH, widgetW, widgetH});

    m_cpuStatsWidget.setViewport(Viewport{windowWidth - widgetW - marginX,
            windowHeight/2 - windowHeight/4, widgetW, sideWidgetH});

    m_processWidget.setViewport(Viewport{ marginX + windowWidth/2 - widgetW,
            marginY, 2*widgetW, widgetH });

    m_graphWidget.setViewport(Viewport{ marginX, windowHeight/2 -
            windowHeight/4, widgetW, windowHeight/2 });

    m_systemStatsWidget.setViewport(Viewport{ marginX, marginY, widgetW,
            widgetH });

    m_mainWidget.setViewport(Viewport{ marginX + windowWidth/2 - widgetW,
            windowHeight/2 - windowHeight/4, 2 * widgetW, sideWidgetH });

    m_musicWidget.setViewport(Viewport{ windowWidth - widgetW - marginX,
            marginY, widgetW, widgetH});
}

void Renderer::initVBOs() {

    // Graph background grid VBO:
    {
        constexpr auto numVertLines = size_t{ 14U };
        constexpr auto numHorizLines = size_t{ 7U };

        auto gVerts = std::vector<GLfloat>{};
        auto gIndices = std::vector<GLuint>{};
        gVerts.reserve(4 * (numVertLines + numHorizLines) );
        gIndices.reserve(2 * (numVertLines + numHorizLines) );

        // Fill the vertex and index arrays with data for drawing grid as VBO
        for (auto i = size_t{ 0U }; i < numVertLines; ++i) {
            const float x{ (i)/static_cast<float>(numVertLines-1) * 2.0f - 1.0f };
            gVerts.push_back(x);
            gVerts.push_back(1.0f); // Vertical line top vert

            gVerts.push_back(x);
            gVerts.push_back(-1.0f); // Vertical line bottom vert

            gIndices.push_back(2*i);
            gIndices.push_back(2*i+1);
        }

        // Offset value for the index array
        const auto vertLineIndexCount{ gIndices.size() };
        for (auto i = size_t{ 0U }; i < numHorizLines; ++i) {
            const float y{ static_cast<float>(i)/(numHorizLines-1) * 2.0f - 1.0f };
            gVerts.push_back(-1.0f);
            gVerts.push_back(y); // Horizontal line bottom vert

            gVerts.push_back(1.0f);
            gVerts.push_back(y); // Horizontal line top vert

            gIndices.push_back(vertLineIndexCount + 2*i);
            gIndices.push_back(vertLineIndexCount + 2*i+1);
        }
        graphIndicesSize = gIndices.size();

        // Initialise the graph grid VBO
        glGenBuffers(1, &graphGridVertsID);
        glBindBuffer(GL_ARRAY_BUFFER, graphGridVertsID);
        glBufferData(GL_ARRAY_BUFFER, gVerts.size() * sizeof(GLfloat),
                     gVerts.data(), GL_STATIC_DRAW);

        // Initialise graph grid index array
        glGenBuffers(1, &graphGridIndicesID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, graphGridIndicesID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, gIndices.size() * sizeof(GLuint),
                     gIndices.data(), GL_STATIC_DRAW);
    }

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderer::initShaders() {
    m_cpuGraphShader = loadShader("cpuGraph.vert", "cpuGraph.frag");

    m_graphAlphaLoc = glGetUniformLocation(m_cpuGraphShader, "lineAlpha");

    if (m_graphAlphaLoc == -1) {
        //std::cout << "Failed to get uniform location for \'lineAlpha\'\n";
    }
}

}
