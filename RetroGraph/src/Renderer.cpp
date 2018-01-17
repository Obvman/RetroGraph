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
    m_settings{ &settings },
    m_fontManager{ w.getHwnd(), w.getHeight() },
    m_timeWidget{ &m_fontManager, &_rg.getCPUMeasure(), &_rg.getNetMeasure(),
                  m_settings->isVisible(RG_WIDGET_TIME) },
    m_hddWidget{ &m_fontManager, _rg.getDriveMeasure(), 
                 m_settings->isVisible(RG_WIDGET_DRIVES) },
    m_cpuStatsWidget{ &m_fontManager, &_rg.getCPUMeasure(),
                      m_settings->isVisible(RG_WIDGET_CPU_STATS) },
    m_processCPUWidget{ &m_fontManager, &_rg.getProcessMeasure(), 
                        m_settings->isVisible(RG_WIDGET_PROCESSES_CPU) },
    m_processRAMWidget{ &m_fontManager, &_rg.getProcessMeasure(), 
                        m_settings->isVisible(RG_WIDGET_PROCESSES_RAM) },
    m_graphWidget{ &m_fontManager, &_rg.getCPUMeasure(), &_rg.getRAMMeasure(),
                   &_rg.getNetMeasure(), &_rg.getGPUMeasure(), 
                   m_settings->isVisible(RG_WIDGET_GRAPHS) },
    m_systemStatsWidget{ &m_fontManager, &_rg.getSystemMeasure(),
                         &_rg.getCPUMeasure(), &_rg.getNetMeasure(),
                         m_settings->isVisible(RG_WIDGET_SYSTEM_STATS) },
    m_mainWidget{ &m_fontManager, _rg.getAnimationState(), m_settings->isVisible(RG_WIDGET_MAIN) },
    m_musicWidget{ &m_fontManager, _rg.getMusicMeasure(), 
                   m_settings->isVisible(RG_WIDGET_MUSIC) } {

    setViewports(w.getWidth(), w.getHeight());

    initVBOs();
    initShaders();
}

Renderer::~Renderer() {
    glDeleteBuffers(1, &m_graphGridVertsID);
    glDeleteBuffers(1, &m_graphGridIndicesID);
}

void Renderer::draw(uint32_t ticks) const {
    // Render the bulk of widgets at a low FPS to keep light on resources
    constexpr auto framesPerSecond = uint32_t{ 2U };
    if ((ticks % std::lround(
        static_cast<float>(rg::ticksPerSecond)/framesPerSecond)) == 0) {

        glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, BGCOLOR_A);

        m_timeWidget.draw();
        m_hddWidget.draw();
        m_cpuStatsWidget.draw();
        m_processCPUWidget.draw();
        m_processRAMWidget.draw();
        m_graphWidget.draw();
        m_systemStatsWidget.draw();
        m_musicWidget.draw();
    }

    constexpr auto mainWidgetFPS = uint32_t{ animationFPS };
    if ((ticks % std::lround(
        static_cast<float>(rg::ticksPerSecond) / mainWidgetFPS)) == 0) {

        m_mainWidget.draw();
    }
}

void Renderer::updateWindowSize(int32_t newWidth, int32_t newHeight) {
    setViewports(newWidth, newHeight);
    m_fontManager.refreshFonts(newHeight);

    m_systemStatsWidget.needsRedraw();
}

void Renderer::setWidgetVisibility(Widget w, bool v) {
    switch (w) {
        case Widget::CPUStats:
            m_cpuStatsWidget.setVisibility(v);
            break;
        case Widget::Time:
            m_timeWidget.setVisibility(v);
            break;
        case Widget::HDD:
            m_hddWidget.setVisibility(v);
            break;
        case Widget::ProcessRAM:
            m_processRAMWidget.setVisibility(v);
            break;
        case Widget::ProcessCPU:
            m_processCPUWidget.setVisibility(v);
            break;
        case Widget::Graph:
            m_graphWidget.setVisibility(v);
            break;
        case Widget::SystemStats:
            m_systemStatsWidget.setVisibility(v);
            break;
        case Widget::Main:
            m_mainWidget.setVisibility(v);
            break;
        case Widget::Music:
            m_musicWidget.setVisibility(v);
            break;
    }
}

/********************* Private Functions ********************/

void Renderer::setViewports(int32_t windowWidth, int32_t windowHeight) {
    // For positions that can contain multiple widgets, track how many widgets
    // will fill that position. Currently on the horizontal center positions
    // can contain multiple widgets
    std::vector<int32_t> positionFills( 2, 0 );

    m_timeWidget.setViewport(calcViewport(
            m_settings->getWidgetPosition(RG_WIDGET_TIME),
            windowWidth, windowHeight, positionFills));

    m_hddWidget.setViewport(calcViewport(
            m_settings->getWidgetPosition(RG_WIDGET_DRIVES),
            windowWidth, windowHeight, positionFills));

    m_cpuStatsWidget.setViewport(calcViewport(
            m_settings->getWidgetPosition(RG_WIDGET_CPU_STATS),
            windowWidth, windowHeight, positionFills));

    m_graphWidget.setViewport(calcViewport(
            m_settings->getWidgetPosition(RG_WIDGET_GRAPHS),
            windowWidth, windowHeight, positionFills));

    m_systemStatsWidget.setViewport(calcViewport(
            m_settings->getWidgetPosition(RG_WIDGET_SYSTEM_STATS),
            windowWidth, windowHeight, positionFills));

    m_mainWidget.setViewport(calcViewport(
            m_settings->getWidgetPosition(RG_WIDGET_MAIN),
            windowWidth, windowHeight, positionFills));

    m_musicWidget.setViewport(calcViewport(
            m_settings->getWidgetPosition(RG_WIDGET_MUSIC),
            windowWidth, windowHeight, positionFills));

    m_processCPUWidget.setViewport(calcViewport(
            m_settings->getWidgetPosition(RG_WIDGET_PROCESSES_CPU),
            windowWidth, windowHeight, positionFills));

    m_processRAMWidget.setViewport(calcViewport(
            m_settings->getWidgetPosition(RG_WIDGET_PROCESSES_RAM),
            windowWidth, windowHeight, positionFills));

    if (positionFills[0] > 2) {
        fatalMessageBox("You put too many widgets in the top-middle area!");
    } else if (positionFills[1] > 2) {
        fatalMessageBox("You put too many widgets in the bottom-middle area!");
    }
}

Viewport Renderer::calcViewport(WidgetPosition pos,
                                int32_t windowWidth, int32_t windowHeight,
                                std::vector<int32_t>& positionFills) {
    const auto widgetW{ windowWidth/5 };
    const auto widgetH{ windowHeight/6 };
    const auto sideWidgetH{ windowHeight/2 };

    // For positions that fit multiple widgets, set the appropriate viewport
    // and increment the number of widgets in that position
    switch (pos) {
        case WidgetPosition::TOP_MID:
            ++positionFills[0];
            return Viewport{ 
                marginX + windowWidth/2 - widgetW + widgetW * (positionFills[0]-1),
                windowHeight - marginY - widgetH,
                widgetW,
                widgetH };
        case WidgetPosition::BOT_MID:
            ++positionFills[1];
            return Viewport{
                marginX + windowWidth/2 - widgetW + widgetW * (positionFills[1]-1),
                marginY,
                widgetW,
                widgetH };
        case WidgetPosition::MID_MID:
            return Viewport{ 
                marginX + windowWidth/2 - widgetW,
                windowHeight/2 - windowHeight/4,
                2 * widgetW,
                sideWidgetH };
        // Positions that only take a single widget
        case WidgetPosition::TOP_LEFT:
            return Viewport{  marginX, windowHeight - marginY -
                widgetH, widgetW, widgetH };
        case WidgetPosition::TOP_RIGHT:
            return Viewport{ windowWidth - widgetW - marginX,
                windowHeight - marginY - widgetH, widgetW, widgetH }; 
        case WidgetPosition::MID_LEFT:
            return Viewport{ marginX, windowHeight/2 -
                windowHeight/4, widgetW, sideWidgetH };
        case WidgetPosition::MID_RIGHT:
            return Viewport{ windowWidth - widgetW - marginX,
                windowHeight/2 - windowHeight/4, widgetW, sideWidgetH };
        case WidgetPosition::BOT_LEFT:
            return Viewport{ marginX, marginY, widgetW, widgetH };
        case WidgetPosition::BOT_RIGHT:
            return Viewport{ windowWidth - widgetW - marginX,
                    marginY, widgetW, widgetH };
        default:
            return Viewport{};
    }
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
