#include "../headers/Renderer.h"

#include "../headers/RetroGraph.h"
#include "../headers/Window.h"
#include "../headers/CPUMeasure.h"
#include "../headers/GPUMeasure.h"
#include "../headers/RAMMeasure.h"
#include "../headers/NetMeasure.h"
#include "../headers/ProcessMeasure.h"
#include "../headers/DriveMeasure.h"
#include "../headers/SystemMeasure.h"
#include "../headers/UserSettings.h"
#include "../headers/colors.h"
#include "../headers/utils.h"
#include "../headers/GLShaders.h"

namespace rg {

Renderer::Renderer(const Window& w, const RetroGraph& _rg,
                   const UserSettings& settings) :
    m_renderTargetHandle{ w.getHwnd() },
    m_fontManager{ w.getHwnd(), w.getHeight() },
    m_timeWidget{ &m_fontManager, &_rg.getCPUMeasure(), &_rg.getNetMeasure(),
                  settings.isVisible(RG_WIDGET_TIME) },
    m_hddWidget{ &m_fontManager, &_rg.getDriveMeasure(), 
                 settings.isVisible(RG_WIDGET_DRIVES) },
    m_cpuStatsWidget{ &m_fontManager, &_rg.getCPUMeasure(),
                      settings.isVisible(RG_WIDGET_CPU_STATS) },
    m_processWidget{ &m_fontManager, &_rg.getProcessMeasure(), 
                     settings.isVisible(RG_WIDGET_PROCESSES) },
    m_graphWidget{ &m_fontManager, &_rg.getCPUMeasure(), &_rg.getRAMMeasure(),
                   &_rg.getNetMeasure(), &_rg.getGPUMeasure(), 
                   settings.isVisible(RG_WIDGET_GRAPHS) },
    m_systemStatsWidget{ &m_fontManager, &_rg.getSystemMeasure(),
                         &_rg.getCPUMeasure(), &_rg.getNetMeasure(),
                         settings.isVisible(RG_WIDGET_SYSTEM_STATS) },
    m_mainWidget{ &m_fontManager, settings.isVisible(RG_WIDGET_MAIN) },
    m_musicWidget{ &m_fontManager, &_rg.getMusicMeasure(), 
                   settings.isVisible(RG_WIDGET_MUSIC) } {

    setViewports(w.getWidth(), w.getHeight());

    initVBOs();
    initShaders();
}

Renderer::~Renderer() {
    glDeleteBuffers(1, &m_graphGridVertsID);
    glDeleteBuffers(1, &m_graphGridIndicesID);
}

void Renderer::draw(uint32_t ticks) const {
    constexpr auto framesPerSecond = uint32_t{ 2U };
    if ((ticks % std::lround(
        static_cast<float>(rg::ticksPerSecond)/framesPerSecond)) == 0) {

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
}

void Renderer::updateWindowSize(int32_t newWidth, int32_t newHeight) {
    setViewports(newWidth, newHeight);
    m_fontManager.refreshFonts(newHeight);

    m_systemStatsWidget.needsRedraw();
}

/********************* Private Functions ********************/

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
