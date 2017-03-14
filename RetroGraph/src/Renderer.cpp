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
#include "../headers/colors.h"
#include "../headers/utils.h"
#include "../headers/GLShaders.h"

namespace rg {
// Automatically binds/unbinds given VBOs and executes the function given
template<typename F>
void vboDrawScope(GLuint vertID, GLuint indexID, F f) {
    glBindBuffer(GL_ARRAY_BUFFER, vertID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);

    f();

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Renderer::Renderer() :
    m_renderTargetHandle{ nullptr },
    m_cpuMeasure{ nullptr },
    m_gpuMeasure{ nullptr },
    m_ramMeasure{ nullptr },
    m_netMeasure{ nullptr },
    m_processMeasure{ nullptr },
    m_driveMeasure{ nullptr },
    m_sysInfo{ nullptr },
    m_statsStrings{},
    m_fontManager{}
{ /* Empty */ }

Renderer::~Renderer() {
}

void Renderer::init(HWND hWnd, uint32_t windowWidth, uint32_t windowHeight,
                    const CPUMeasure& _cpu, const GPUMeasure& _gpu,
                    const RAMMeasure& _ram, const NetMeasure& _net,
                    const ProcessMeasure& _proc, const DriveMeasure& _drive,
                    const SystemInfo& _sys) {
    m_renderTargetHandle = hWnd;
    m_cpuMeasure = &_cpu;
    m_gpuMeasure = &_gpu;
    m_ramMeasure = &_ram;
    m_netMeasure = &_net;
    m_processMeasure = &_proc;
    m_driveMeasure = &_drive;
    m_sysInfo = &_sys;

    // Initialise static statistics strings for drawing stats widget
    m_statsStrings.emplace_back(m_sysInfo->getUserName() + "@" +
                                m_sysInfo->getComputerName());
    m_statsStrings.emplace_back(m_sysInfo->getOSInfoStr());
    m_statsStrings.emplace_back(m_cpuMeasure->getCPUName());
    m_statsStrings.emplace_back(m_sysInfo->getGPUDescription());
    m_statsStrings.emplace_back(m_sysInfo->getRAMDescription());
    m_statsStrings.emplace_back("DNS: " + m_netMeasure->getDNS());
    m_statsStrings.emplace_back("Hostname: " + m_netMeasure->getHostname());
    m_statsStrings.emplace_back("MAC: " + m_netMeasure->getAdapterMAC());
    m_statsStrings.emplace_back("LAN IP: " + m_netMeasure->getAdapterIP());

    m_fontManager.init(hWnd, windowWidth, windowHeight);
    initViewportBuffers(windowWidth, windowHeight);
    initVBOs();
    initShaders();
}

void Renderer::draw(uint32_t ticks) const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, BGCOLOR_A);

    drawLeftGraphWidget();

    drawProcessWidget();
    drawTimeWidget();
    drawHDDWidget();
    drawStatsWidget();

    drawRightGraphWidget();
}

void Renderer::release() {
    // Free VBO memory
    glDeleteBuffers(1, &m_graphGridVertsID);
    glDeleteBuffers(1, &m_graphGridIndicesID);

    m_fontManager.release();
}

/********************* Private Functions ********************/
void Renderer::initViewportBuffers(uint32_t windowWidth, uint32_t windowHeight) {
    m_timeWidgetVP[0] = marginX;
    m_timeWidgetVP[1] = 5 * windowHeight/6 - marginY;
    m_timeWidgetVP[2] = windowWidth/5;
    m_timeWidgetVP[3] = windowHeight/6; // Top left

    m_hddWidgetVP[0] = windowWidth - windowWidth/5 - marginX,
    m_hddWidgetVP[1] = 5 * windowHeight/6 - marginY,
    m_hddWidgetVP[2] = windowWidth/5,
    m_hddWidgetVP[3] = windowHeight/6; // Top right

    m_procWidgetVP[0] = marginX + windowWidth/2 - windowWidth/5;
    m_procWidgetVP[1] = marginY;
    m_procWidgetVP[2] = 2*windowWidth/5;
    m_procWidgetVP[3] = windowHeight/6; // Bottom middle

    m_statsWidgetVP[0] = marginX;
    m_statsWidgetVP[1] = marginY;
    m_statsWidgetVP[2] = windowWidth/5;
    m_statsWidgetVP[3] = windowHeight/6; // Bottom left

    m_leftGraphWidgetVP[0] = marginX;
    m_leftGraphWidgetVP[1] = windowHeight/4 + 2*marginY;
    m_leftGraphWidgetVP[2] = windowWidth/5;
    m_leftGraphWidgetVP[3] = windowHeight/2; // Left-Mid

    m_cpuGraphVP[0] = m_leftGraphWidgetVP[0];
    m_cpuGraphVP[1] = m_leftGraphWidgetVP[1] + 3*m_leftGraphWidgetVP[3]/4;
    m_cpuGraphVP[2] = m_leftGraphWidgetVP[2];
    m_cpuGraphVP[3] = m_leftGraphWidgetVP[3]/4;

    m_ramGraphVP[0] = m_leftGraphWidgetVP[0];
    m_ramGraphVP[1] = m_leftGraphWidgetVP[1] + 2*m_leftGraphWidgetVP[3]/4;
    m_ramGraphVP[2] = m_leftGraphWidgetVP[2];
    m_ramGraphVP[3] = m_leftGraphWidgetVP[3]/4;

    m_gpuGraphVP[0] = m_leftGraphWidgetVP[0];
    m_gpuGraphVP[1] = m_leftGraphWidgetVP[1] + 1*m_leftGraphWidgetVP[3]/4;
    m_gpuGraphVP[2] = m_leftGraphWidgetVP[2];
    m_gpuGraphVP[3] = m_leftGraphWidgetVP[3]/4;

    m_netGraphVP[0] = m_leftGraphWidgetVP[0];
    m_netGraphVP[1] = m_leftGraphWidgetVP[1] + 0*m_leftGraphWidgetVP[3]/4;
    m_netGraphVP[2] = m_leftGraphWidgetVP[2];
    m_netGraphVP[3] = m_leftGraphWidgetVP[3]/4;

    m_mainWidgetVP[0] = marginX + windowWidth/2 - windowWidth/5;
    m_mainWidgetVP[1] = 2*marginY + windowHeight/4;
    m_mainWidgetVP[2] = 2 * windowWidth/5;
    m_mainWidgetVP[3] = windowHeight/2; // Midd;

    m_rightGraphWidgetVP[0] = windowWidth - windowWidth/5 - marginX;
    m_rightGraphWidgetVP[1] = windowHeight/4 + 2*marginY;
    m_rightGraphWidgetVP[2] = windowWidth/5;
    m_rightGraphWidgetVP[3] = windowHeight/2;

    m_coreGraphsVP[0] = m_rightGraphWidgetVP[0];
    m_coreGraphsVP[1] = m_rightGraphWidgetVP[1];
    m_coreGraphsVP[2] = m_rightGraphWidgetVP[2];
    m_coreGraphsVP[3] = m_rightGraphWidgetVP[3]/2;
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
            gVerts.push_back(y); // Horizontal line bottom vert

            gIndices.push_back(vertLineIndexCount + 2*i);
            gIndices.push_back(vertLineIndexCount + 2*i+1);
        }
        m_graphIndicesSize = gIndices.size();

        // Initialise the graph grid VBO
        glGenBuffers(1, &m_graphGridVertsID);
        glBindBuffer(GL_ARRAY_BUFFER, m_graphGridVertsID);
        glBufferData(GL_ARRAY_BUFFER, gVerts.size() * sizeof(GLfloat),
                     gVerts.data(), GL_STATIC_DRAW);

        // Initialise graph grid index array
        glGenBuffers(1, &m_graphGridIndicesID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_graphGridIndicesID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, gIndices.size() * sizeof(GLuint),
                     gIndices.data(), GL_STATIC_DRAW);
    }

    { // VBO for CPU line graph
        /*glGenBuffers(1, &m_graphLineVertsID);
        glBindBuffer(GL_ARRAY_BUFFER, m_graphLineVertsID);
        glBufferData(GL_ARRAY_BUFFER, m_cpuMeasure.getUsageData().size() * sizeof(GLfloat),
                     m_cpuMeasure.getUsageData().data(), GL_DYNAMIC_DRAW);*/
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

void Renderer::drawFilledGraph(const std::vector<float> data) const {
    glBegin(GL_QUADS); {
        for (auto i = size_t{ 0U }; i < data.size() - 1; ++i) {
            glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.7f);

            const auto x1 = float{ (static_cast<float>(i) / (data.size() - 1)) * 2.0f - 1.0f };
            const auto y1 = float{ data[i] * 2.0f - 1.0f };
            const auto x2 = float{ (static_cast<float>(i+1) / (data.size() - 1)) * 2.0f - 1.0f };
            const auto y2 = float{ data[i+1] * 2.0f - 1.0f };

            glVertex2f(x1, -1.0f); // Bottom-left
            glVertex2f(x1, y1); // Top-left
            glVertex2f(x2, y2); // Top-right
            glVertex2f(x2, -1.0f); // Bottom-right
        }
    } glEnd();
}

void Renderer::drawLineGraph(const std::vector<float> data) const {
    glBegin(GL_LINE_STRIP); {
        // Draw each node in the graph
        for (auto i = size_t{ 0U }; i < data.size(); ++i) {
            glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, GRAPHLINE_A);

            const auto x = float{ (static_cast<float>(i) / (data.size() - 1))
                                   * 2.0f - 1.0f };
            const auto y = float{ data[i] * 2.0f - 1.0f };

            glVertex2f(x, y);
        }
    } glEnd();
}

void Renderer::drawGraphGrid() const {
    // Draw the background grid for the graph
    vboDrawScope(m_graphGridVertsID, m_graphGridIndicesID, [this]() {
        glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.2f);
        glLineWidth(0.5f);
        glDrawElements(GL_LINES, m_graphIndicesSize, GL_UNSIGNED_INT, 0);
    });
}

void Renderer::drawMainWidget() const {
    glViewport(m_mainWidgetVP[0], m_mainWidgetVP[1],
               m_mainWidgetVP[2], m_mainWidgetVP[3]);
}

void Renderer::drawRightGraphWidget() const {
    glViewport(m_rightGraphWidgetVP[0], m_rightGraphWidgetVP[1],
               m_rightGraphWidgetVP[2], m_rightGraphWidgetVP[3]);

    drawCoreGraphs();
}

void Renderer::drawCoreGraphs() const {
    glViewport(m_coreGraphsVP[0], m_coreGraphsVP[1],
               m_coreGraphsVP[2], m_coreGraphsVP[3]);

    glLineWidth(0.5f);
    glColor4f(BORDER_R, BORDER_G, BORDER_B, BORDER_A);
    glBegin(GL_LINE_STRIP); {
        glVertex2f(-1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(-1.0f, 1.0f);
    } glEnd();

    // Draw x rows of core graphs, with 2 graphs per row until all graphs
    // are drawn
    const auto numCores{ m_cpuMeasure->getPerCoreUsageData().size() };
    const auto numRows{ numCores / 2 }; // Fair to assume we have an even number of cores

    glLineWidth(0.5f);
    glColor4f(GRAPHLINE_A, GRAPHLINE_G, GRAPHLINE_B, GRAPHLINE_A);
    for (auto i = size_t{ 0U }; i < numCores; ++i) {
        // Set the viewport for the current graph. The y position
        // of each graph changes as we draw more
        glViewport(m_coreGraphsVP[0],
                   (m_coreGraphsVP[1] + (numCores-1)*m_coreGraphsVP[3]/numCores)
                   - i*m_coreGraphsVP[3]/(numCores),
                   m_coreGraphsVP[2],
                   m_coreGraphsVP[3]/numCores);

        drawLineGraph(m_cpuMeasure->getPerCoreUsageData()[i]);

        // Draw the border for this core graph
        glColor4f(BORDER_R, BORDER_G, BORDER_B, BORDER_A);
        glBegin(GL_LINE_STRIP); {
            glVertex2f(-1.0f, 1.0f);
            glVertex2f(1.0f, 1.0f);
            glVertex2f(1.0f, -1.0f);
            glVertex2f(-1.0f, -1.0f);
            glVertex2f(-1.0f, 1.0f);
        } glEnd();

        drawGraphGrid();

        // Draw a label for the core graph
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
        char str[7] = { 'C', 'o', 'r', 'e', ' ', static_cast<char>(i) + '0', '\0'};
        m_fontManager.renderText(-0.93f, 0.80f, RG_FONT_SMALL, str, sizeof(str));
    }
}

void Renderer::drawLeftGraphWidget() const {
    glViewport(m_leftGraphWidgetVP[0], m_leftGraphWidgetVP[1],
               m_leftGraphWidgetVP[2], m_leftGraphWidgetVP[3]);

    drawCpuGraph();
    drawRamGraph();
    drawGpuGraph();
    drawNetGraph();
}

void Renderer::drawCpuGraph() const {
    { // Graph grid/border
        glViewport(m_cpuGraphVP[0], m_cpuGraphVP[1],
                   m_cpuGraphVP[2], m_cpuGraphVP[3]);

        // Draw border
        glLineWidth(0.5f);
        glColor4f(BORDER_R, BORDER_G, BORDER_B, BORDER_A);
        glBegin(GL_LINE_STRIP); {
            glVertex2f(-1.0f, 1.0f);
            glVertex2f(1.0f, 1.0f);
            glVertex2f(1.0f, -1.0f);
            glVertex2f(-1.0f, -1.0f);
            glVertex2f(-1.0f, 1.0f);
        } glEnd();
    }

    // Set the viewport for the graph to be left section
    glViewport(m_cpuGraphVP[0], m_cpuGraphVP[1],
               (m_cpuGraphVP[2] * 4)/5, m_cpuGraphVP[3]);

    drawGraphGrid();
    glLineWidth(0.5f);
    drawLineGraph(m_cpuMeasure->getUsageData());

    { // Text
        glViewport(m_cpuGraphVP[0] + (4 * m_cpuGraphVP[2]) / 5, m_cpuGraphVP[1],
                   m_cpuGraphVP[2] / 5, m_cpuGraphVP[3]);
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

        const char* str{ "Sys Load" };
        const char* min{ "0%" };
        const char* max{ "100%" };
        m_fontManager.renderText(-0.8f, -0.02f, RG_FONT_SMALL, str);
        m_fontManager.renderText(-0.8f, -0.77f, RG_FONT_SMALL, min);
        m_fontManager.renderText(-0.8f, 0.70f, RG_FONT_SMALL, max);
    }
}

void Renderer::drawRamGraph() const {
    glViewport(m_ramGraphVP[0], m_ramGraphVP[1],
               m_ramGraphVP[2] , m_ramGraphVP[3]);
    // Draw border
    glLineWidth(0.5f);
    glColor4f(BORDER_R, BORDER_G, BORDER_B, BORDER_A);
    glBegin(GL_LINE_STRIP); {
        glVertex2f(-1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(-1.0f, 1.0f);
    } glEnd();

    // Set the viewport for the graph to be left section
    glViewport(m_ramGraphVP[0], m_ramGraphVP[1],
               (m_ramGraphVP[2]*4)/5 , m_ramGraphVP[3]);

    // Draw the background grid for the graph
    drawGraphGrid();

    {// Draw the line graph
        glLineWidth(0.5f);
        drawLineGraph(m_ramMeasure->getUsageData());
    }

    {// Set viewport for text drawing
        glViewport(m_ramGraphVP[0] + (4 * m_ramGraphVP[2]) / 5, m_ramGraphVP[1],
                   m_ramGraphVP[2] / 5, m_ramGraphVP[3]);
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

        const char* str{ "RAM Load" };
        const char* min{ "0%" };
        const char* max{ "100%" };
        m_fontManager.renderText(-0.8f, -0.02f, RG_FONT_SMALL, str);
        m_fontManager.renderText(-0.8f, -0.77f, RG_FONT_SMALL, min);
        m_fontManager.renderText(-0.8f, 0.70f, RG_FONT_SMALL, max);
        /*fontScope(smlFontBase, []() {
            glRasterPos2f(-0.8f, -0.02f);
            const char* str{ "RAM Load" };
            glCallLists(strlen(str), GL_UNSIGNED_BYTE, str);

            glRasterPos2f(-0.8f, -0.77f);
            const char* min{ "0%" };
            glCallLists(strlen(min), GL_UNSIGNED_BYTE, min);

            glRasterPos2f(-0.8f, 0.70f);
            const char* max{ "100%" };
            glCallLists(strlen(max), GL_UNSIGNED_BYTE, max);
        });*/
    }
}

void Renderer::drawGpuGraph() const {
    glViewport(m_gpuGraphVP[0], m_gpuGraphVP[1],
               m_gpuGraphVP[2] , m_gpuGraphVP[3]);
    // Draw border
    glLineWidth(0.5f);
    glColor4f(BORDER_R, BORDER_G, BORDER_B, BORDER_A);
    glBegin(GL_LINE_STRIP); {
        glVertex2f(-1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(-1.0f, 1.0f);
    } glEnd();

    // Set the viewport for the graph to be left section
    glViewport(m_gpuGraphVP[0], m_gpuGraphVP[1],
               (m_gpuGraphVP[2]*4)/5 , m_gpuGraphVP[3]);

    // Draw the background grid for the graph
    drawGraphGrid();

    // Draw the line graph
    glLineWidth(0.5f);
    drawLineGraph(m_gpuMeasure->getUsageData());

    {// Set viewport for text drawing
        glViewport(m_gpuGraphVP[0] + (4 * m_gpuGraphVP[2]) / 5, m_gpuGraphVP[1],
                   m_gpuGraphVP[2] / 5, m_gpuGraphVP[3]);
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

        const char* str{ "GPU Load" };
        const char* min{ "0%" };
        const char* max{ "100%" };
        m_fontManager.renderText(-0.8f, -0.02f, RG_FONT_SMALL, str);
        m_fontManager.renderText(-0.8f, -0.77f, RG_FONT_SMALL, min);
        m_fontManager.renderText(-0.8f, 0.70f, RG_FONT_SMALL, max);
        /*fontScope(smlFontBase, []() {
            glRasterPos2f(-0.8f, -0.02f);
            const char* str{ "GPU Load" };
            glCallLists(strlen(str), GL_UNSIGNED_BYTE, str);

            glRasterPos2f(-0.8f, -0.77f);
            const char* min{ "0%" };
            glCallLists(strlen(min), GL_UNSIGNED_BYTE, min);

            glRasterPos2f(-0.8f, 0.70f);
            const char* max{ "100%" };
            glCallLists(strlen(max), GL_UNSIGNED_BYTE, max);
        });*/
    }

}

void Renderer::drawNetGraph() const {
    glViewport(m_netGraphVP[0], m_netGraphVP[1],
               m_netGraphVP[2] , m_netGraphVP[3]);
    // Draw border
    glLineWidth(0.5f);
    glColor4f(BORDER_R, BORDER_G, BORDER_B, BORDER_A);
    glBegin(GL_LINE_STRIP); {
        glVertex2f(-1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(-1.0f, 1.0f);
    } glEnd();

    // Set the viewport for the graph to be left section
    glViewport(m_netGraphVP[0], m_netGraphVP[1],
               (m_netGraphVP[2]*4)/5 , m_netGraphVP[3]);

    // Draw the background grid for the graph
    drawGraphGrid();

    {// Draw the line graph
        glLineWidth(0.5f);
        const auto& downData{ m_netMeasure->getDownData() };
        const auto& upData{ m_netMeasure->getUpData() };
        const auto maxDownValMB{ m_netMeasure->getMaxDownValue() /
                                 static_cast<float>(MB) };
        const auto maxUpValMB{ m_netMeasure->getMaxUpValue() /
                               static_cast<float>(MB) };

        const auto maxValMB{ max(maxUpValMB, maxDownValMB) };

        // Draw the download graph
        glBegin(GL_LINE_STRIP); {
            glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.7f);
            for (auto i = size_t{ 0U }; i < downData.size() - 1; ++i) {
                const auto percent1 = float{ (downData[i] / static_cast<float>(MB)) / maxValMB };
                const auto percent2 = float{ (downData[i+1] / static_cast<float>(MB)) / maxValMB };

                const auto x1 = float{ (static_cast<float>(i) / (downData.size() - 1)) * 2.0f - 1.0f };
                const auto y1 = float{ percent1 * 2.0f - 1.0f };
                const auto x2 = float{ (static_cast<float>(i+1) / (downData.size() - 1)) * 2.0f - 1.0f };
                const auto y2 = float{ percent2 * 2.0f - 1.0f };

                glVertex2f(x1, y1);    // Top-left
                glVertex2f(x2, y2);    // Top-right
            }
        } glEnd();

        // Draw the upload graph
        glBegin(GL_LINE_STRIP); {
            glColor4f(PINK1_R, PINK1_G, PINK1_B, 0.7f);
            for (auto i = size_t{ 0U }; i < upData.size() - 1; ++i) {
                const auto percent1 = float{ (upData[i] / static_cast<float>(MB)) / maxValMB };
                const auto percent2 = float{ (upData[i+1] / static_cast<float>(MB)) / maxValMB };

                const auto x1 = float{ (static_cast<float>(i) / (upData.size() - 1)) * 2.0f - 1.0f };
                const auto y1 = float{ percent1 * 2.0f - 1.0f };
                const auto x2 = float{ (static_cast<float>(i+1) / (upData.size() - 1)) * 2.0f - 1.0f };
                const auto y2 = float{ percent2 * 2.0f - 1.0f };

                glVertex2f(x1, y1); // Top-left
                glVertex2f(x2, y2); // Top-right
            }
        } glEnd();
    }

    {// Text
        glViewport(m_netGraphVP[0] + (4 * m_netGraphVP[2]) / 5, m_netGraphVP[1],
                   m_netGraphVP[2] / 5, m_netGraphVP[3]);
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

        const auto maxVal{ m_netMeasure->getMaxDownValue() };

        std::string suffix{ "B" };
        if (maxVal > 1000) {
            suffix = "KB";
        }
        if (maxVal > 1000 * 1000) {
            suffix = "MB";
        }

        const char* str{ "Down/Up" };
        const auto bottom{ "0" + suffix };
        const char* max{ "100%" };
        m_fontManager.renderText(-0.8f, -0.02f, RG_FONT_SMALL, str);
        m_fontManager.renderText(-0.8f, -0.77f, RG_FONT_SMALL, bottom.c_str(), bottom.size());

        /* Print the maximum throughput as the scale at the top of the graph */
        if (suffix == "MB") {
            char buff[8];
            snprintf(buff, sizeof(buff), "%5.1fMB", maxVal/static_cast<float>(MB));
            m_fontManager.renderText(-0.8f, 0.70f, RG_FONT_SMALL, buff, sizeof(buff));
        } else if (suffix == "KB") {
            char buff[8];
            snprintf(buff, sizeof(buff), "%5.1fKB", maxVal/static_cast<float>(KB));
            m_fontManager.renderText(-0.8f, 0.70f, RG_FONT_SMALL, buff, sizeof(buff));
        } else {
            const auto top{ std::to_string(maxVal) + suffix };
            char buff[5];
            snprintf(buff, sizeof(buff), "%3lluB", maxVal);
            m_fontManager.renderText(-0.8f, 0.70f, RG_FONT_SMALL, buff, sizeof(buff));
        }
    }
}

void Renderer::drawProcessWidget() const {
    glViewport(m_procWidgetVP[0], m_procWidgetVP[1],
               m_procWidgetVP[2], m_procWidgetVP[3]);
    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);
    glLineWidth(0.5f);

    // Draw dividers
    glBegin(GL_LINES); {
        glVertex2f(-1.0f, 1.0f);
        glVertex2f( 1.0f, 1.0f); // Top line

        glVertex2f(-1.0f, -1.0f);
        glVertex2f( 1.0f, -1.0f); // Bottom line

        glVertex2f(0.0f, -1.0f);
        glVertex2f(0.0f,  1.0f); // Middle line
    } glEnd();

    drawProcCPUList();
    drawProcRAMList();
}

void Renderer::drawProcCPUList() const {
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    const auto rasterX = float{ -0.95f };
    auto rasterY = float{ 0.79f }; // Y changes for each process drawn

    for (const auto& pair : m_processMeasure->getProcCPUData()) {
        // Draw the process name
        glRasterPos2f(rasterX, rasterY);
        glCallLists(pair.first.length(), GL_UNSIGNED_BYTE, pair.first.c_str());

        // Draw the process's CPU percentage
        glRasterPos2f(-0.13f, rasterY);
        char buff[6];
        snprintf(buff, sizeof(buff), "%4.1f%%", pair.second);
        glCallLists(sizeof(buff), GL_UNSIGNED_BYTE, buff);

        rasterY -= 1.8f / (m_processMeasure->getProcCPUData().size());
    }
}

void Renderer::drawProcRAMList() const {
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    const auto rasterX = float{ 0.05f };
    auto rasterY = float{ 0.79f }; // Y changes for each process drawn

    for (const auto& pair : m_processMeasure->getProcRAMData()) {
        // Draw the process name
        glRasterPos2f(rasterX, rasterY);
        glCallLists(pair.first.length(), GL_UNSIGNED_BYTE, pair.first.c_str());

        // Draw the process's RAM usage
        std::string usage;
        // Format differently if RAM usage is over 1GB
        char buff[6];
        if (pair.second >= 1000) {
            snprintf(buff, sizeof(buff), "%.1fGB", pair.second / 1024.0f);
        } else {
            snprintf(buff, sizeof(buff), "%dMB", pair.second);
        }
        glRasterPos2f(0.84f, rasterY);
        glCallLists(sizeof(buff), GL_UNSIGNED_BYTE, buff);

        rasterY -= 1.8f / (m_processMeasure->getProcCPUData().size());
    }

}

void Renderer::drawStatsWidget() const {
    glViewport(m_statsWidgetVP[0], m_statsWidgetVP[1],
               m_statsWidgetVP[2], m_statsWidgetVP[3]  );

    // Draw dividers
    glColor4f(DIVIDER_R, DIVIDER_G, DIVIDER_B, DIVIDER_A);
    glLineWidth(0.5f);
    glBegin(GL_LINES); {
        glVertex2f(-1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f); // Top

        glVertex2f(-1.0f, -1.0f);
        glVertex2f( 1.0f, -1.0f); // Bottom
    } glEnd();

    const auto numLinesToDraw{ m_statsStrings.size() };
    const auto yRange{ 1.8f };
    const auto rasterX = float{ -0.95f };
    auto rasterY = float{ 0.75f };

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    for (const auto& str : m_statsStrings) {
        glRasterPos2f(rasterX, rasterY);
        glCallLists(str.length(), GL_UNSIGNED_BYTE, str.c_str());
        rasterY -= yRange / numLinesToDraw;
    }
}

void Renderer::drawHDDWidget() const {
    glViewport(m_hddWidgetVP[0], m_hddWidgetVP[1],
               m_hddWidgetVP[2], m_hddWidgetVP[3]);
    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);
    glLineWidth(0.5f);

    // Draw dividers
    glBegin(GL_LINES); {
        glVertex2f(-1.0f, 1.0f); // Drawing at the very edge of the viewport like this leaves an even thinner line
        glVertex2f( 1.0f, 1.0f); // Top line

        glVertex2f(-1.0f, -1.0f);
        glVertex2f( 1.0f, -1.0f); // Bottom line
    } glEnd();

    glPushMatrix();
    const auto numDrives{ m_driveMeasure->getNumDrives() };
    // For each drive, draw a bar and label in the widget
    for (const auto& pdi : m_driveMeasure->getDrives()) {
        drawHDDBar(*pdi);

        // Draw the drive label below the bar
        const char label[3]{ pdi->getDriveLetter(), ':', '\0' };
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
        glRasterPos2f(-0.89f, -0.88f);
        glCallLists(sizeof(label), GL_UNSIGNED_BYTE, label);

        const auto capacityStr{ pdi->getCapacityStr() };

        // Translate by the length of the string to keep it centerred
        glPushMatrix();

        glRasterPos2f(-0.95f, 0.80f);
        glCallLists(capacityStr.size(), GL_UNSIGNED_BYTE, capacityStr.c_str());
        glPopMatrix();

        // Shift the bar to evenly fill the widget
        glTranslatef((2.0f/numDrives), 0.0f, 0.0f);
    }

    glPopMatrix();
}

void Renderer::drawHDDBar(const DriveInfo& di) const {
    const auto percentage{ static_cast<float>(di.getUsedBytes()) / di.getTotalBytes() };

    constexpr auto x = float{ -0.85f };
    constexpr auto yMax = float{ 0.7f };
    constexpr auto yMin = float{ -0.7f };
    constexpr auto yRange{ yMax - yMin };

    glLineWidth(10.0f);

    // Draw the bar
    glBegin(GL_LINES); {
        // Draw the filled section of the bar
        glColor3f(BARFILLED_R, BARFILLED_G, BARFILLED_B);
        glVertex2f(x, yMin);
        glVertex2f(x, yMin + percentage * yRange);

        // Draw the free section of the bar
        glColor3f(BARFREE_R, BARFREE_G, BARFREE_B);
        glVertex2f(x, yMin + percentage * yRange);
        glVertex2f(x, yMax);
    } glEnd();
}

void Renderer::drawTimeWidget() const {
    glViewport(m_timeWidgetVP[0], m_timeWidgetVP[1],
               m_timeWidgetVP[2], m_timeWidgetVP[3]);
    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);
    glLineWidth(0.5f);

    constexpr float leftDivX{ -0.33f };

    {// Draw dividers
        glBegin(GL_LINES); {
            glVertex2f(-1.0f, 1.0f);
            glVertex2f(1.0f, 1.0f); // Top line

            glVertex2f(-1.0f, -1.0f);
            glVertex2f(1.0f, -1.0f); // Bottom line

            glVertex2f(-0.9f, -0.3f);
            glVertex2f(0.9f, -0.3f); // Mid-divider

            glVertex2f(leftDivX, -1.0f);
            glVertex2f(leftDivX, -0.3f); // Left vertical

            glVertex2f(0.33f, -1.0f);
            glVertex2f(0.33f, -0.3f); // Right vertical
        } glEnd();
    }

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    { // Draw the big system time
        time_t now = time(0);
        tm t;
        char buf[9];
        localtime_s(&t, &now);
        strftime(buf, sizeof(buf), "%T", &t);
        m_fontManager.renderText(-0.95f, 0.04f, RG_FONT_TIME, buf, sizeof(buf));

        // Draw the year and month in bottom-middle
        // TODO use full month names
        char dateBuf[7];
        strftime(dateBuf, sizeof(dateBuf), "%d %b", &t);
        m_fontManager.renderText(-0.15f, -0.8f, RG_FONT_STANDARD, dateBuf,
                                 sizeof(dateBuf));

        char yearBuf[5];
        strftime(yearBuf, sizeof(yearBuf), "%Y", &t);
        m_fontManager.renderText(-0.1f, -0.55f, RG_FONT_STANDARD_BOLD, yearBuf,
                                 sizeof(yearBuf));
    }

    // Draw the uptime in bottom-left
    {
        // Spacing between text and the divider
        const int32_t dividerOffset{ 10 };
        const int32_t leftDivPixelPos{
            static_cast<int32_t>(((leftDivX + 1.0f) / 2.0f) * m_timeWidgetVP[2]) };

        // Get the width of the string in pixels, then start drawing the string
        // from a position such that the last character is "dividerOffset" pixels
        // away from the divider
        const char* uptimeTitle{ "UPTIME" };
        int32_t strLenPixels{ m_fontManager.getFontCharWidth(RG_FONT_STANDARD) *
            static_cast<int32_t>(strlen(uptimeTitle)) };
        int32_t strStartPixelX{ leftDivPixelPos - strLenPixels - dividerOffset};

        // Convert pixel value to viewport coordinates
        float strStartVPX{ ((static_cast<float>(strStartPixelX) / m_timeWidgetVP[2]) * 2.0f) - 1.0f};
        m_fontManager.renderText(strStartVPX, -0.55f, RG_FONT_STANDARD, uptimeTitle);

        const auto& uptime{ m_cpuMeasure->getUptimeStr() };

        strLenPixels = static_cast<int32_t>(uptime.size()) * m_fontManager.getFontCharWidth(RG_FONT_STANDARD);
        strStartPixelX = leftDivPixelPos - strLenPixels - dividerOffset;
        strStartVPX =  ((static_cast<float>(strStartPixelX) / m_timeWidgetVP[2]) * 2.0f) - 1.0f;

        glRasterPos2f(strStartVPX, -0.8f);
        glCallLists(uptime.length(), GL_UNSIGNED_BYTE, uptime.c_str());
    }

    // Draw network connection status in bottom-right
    {
        m_fontManager.renderText(0.4f, -0.55f, RG_FONT_STANDARD_BOLD, "NETWORK");

        if (m_netMeasure->isConnected()) {
            m_fontManager.renderText(0.4f, -0.8f, RG_FONT_STANDARD, "UP");
            // TODO print ping in ms
        } else {
            m_fontManager.renderText(0.4f, -0.8f, RG_FONT_STANDARD, "DOWN");
        }
    }
}

}