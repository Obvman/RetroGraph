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
#include "../headers/drawUtils.h"
#include "../headers/GLShaders.h"

namespace rg {

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

    m_fontManager.init(hWnd, windowHeight);
    initViewportBuffers(windowWidth, windowHeight);
    initVBOs();
    initShaders();
}

void Renderer::draw(uint32_t) const {
    //glScissor(m_leftGraphWidgetVP[VP_X], m_leftGraphWidgetVP[VP_Y],
               //m_leftGraphWidgetVP[VP_WIDTH], m_leftGraphWidgetVP[VP_HEIGHT]);
    //glEnable(GL_SCISSOR_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, BGCOLOR_A);
    //glDisable(GL_SCISSOR_TEST);

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
    m_timeWidgetVP[VP_X] = marginX;
    m_timeWidgetVP[VP_Y] = 5 * windowHeight/6 - marginY;
    m_timeWidgetVP[VP_WIDTH] = windowWidth/5;
    m_timeWidgetVP[VP_HEIGHT] = windowHeight/6; // Top left

    m_hddWidgetVP[VP_X] = windowWidth - windowWidth/5 - marginX,
    m_hddWidgetVP[VP_Y] = 5 * windowHeight/6 - marginY,
    m_hddWidgetVP[VP_WIDTH] = windowWidth/5,
    m_hddWidgetVP[VP_HEIGHT] = windowHeight/6; // Top right

    m_procWidgetVP[VP_X] = marginX + windowWidth/2 - windowWidth/5;
    m_procWidgetVP[VP_Y] = marginY;
    m_procWidgetVP[VP_WIDTH] = 2*windowWidth/5;
    m_procWidgetVP[VP_HEIGHT] = windowHeight/6; // Bottom middle

    m_statsWidgetVP[VP_X] = marginX;
    m_statsWidgetVP[VP_Y] = marginY;
    m_statsWidgetVP[VP_WIDTH] = windowWidth/5;
    m_statsWidgetVP[VP_HEIGHT] = windowHeight/6; // Bottom left

    m_leftGraphWidgetVP[VP_X] = marginX;
    m_leftGraphWidgetVP[VP_Y] = windowHeight/4 + 2*marginY;
    m_leftGraphWidgetVP[VP_WIDTH] = windowWidth/5;
    m_leftGraphWidgetVP[VP_HEIGHT] = windowHeight/2; // Left-Mid

    m_cpuGraphVP[VP_X] = m_leftGraphWidgetVP[VP_X];
    m_cpuGraphVP[VP_Y] = m_leftGraphWidgetVP[VP_Y] + 3*m_leftGraphWidgetVP[VP_HEIGHT]/4;
    m_cpuGraphVP[VP_WIDTH] = m_leftGraphWidgetVP[VP_WIDTH];
    m_cpuGraphVP[VP_HEIGHT] = m_leftGraphWidgetVP[VP_HEIGHT]/4;

    m_ramGraphVP[VP_X] = m_leftGraphWidgetVP[VP_X];
    m_ramGraphVP[VP_Y] = m_leftGraphWidgetVP[VP_Y] + 2*m_leftGraphWidgetVP[VP_HEIGHT]/4;
    m_ramGraphVP[VP_WIDTH] = m_leftGraphWidgetVP[VP_WIDTH];
    m_ramGraphVP[VP_HEIGHT] = m_leftGraphWidgetVP[VP_HEIGHT]/4;

    m_gpuGraphVP[VP_X] = m_leftGraphWidgetVP[VP_X];
    m_gpuGraphVP[VP_Y] = m_leftGraphWidgetVP[VP_Y] + 1*m_leftGraphWidgetVP[VP_HEIGHT]/4;
    m_gpuGraphVP[VP_WIDTH] = m_leftGraphWidgetVP[VP_WIDTH];
    m_gpuGraphVP[VP_HEIGHT] = m_leftGraphWidgetVP[VP_HEIGHT]/4;

    m_netGraphVP[VP_X] = m_leftGraphWidgetVP[VP_X];
    m_netGraphVP[VP_Y] = m_leftGraphWidgetVP[VP_Y] + 0*m_leftGraphWidgetVP[VP_HEIGHT]/4;
    m_netGraphVP[VP_WIDTH] = m_leftGraphWidgetVP[VP_WIDTH];
    m_netGraphVP[VP_HEIGHT] = m_leftGraphWidgetVP[VP_HEIGHT]/4;

    m_mainWidgetVP[VP_X] = marginX + windowWidth/2 - windowWidth/5;
    m_mainWidgetVP[VP_Y] = 2*marginY + windowHeight/4;
    m_mainWidgetVP[VP_WIDTH] = 2 * windowWidth/5;
    m_mainWidgetVP[VP_HEIGHT] = windowHeight/2; // Midd;

    m_rightGraphWidgetVP[VP_X] = windowWidth - windowWidth/5 - marginX;
    m_rightGraphWidgetVP[VP_Y] = windowHeight/4 + 2*marginY;
    m_rightGraphWidgetVP[VP_WIDTH] = windowWidth/5;
    m_rightGraphWidgetVP[VP_HEIGHT] = windowHeight/2;

    m_coreGraphsVP[VP_X] = m_rightGraphWidgetVP[VP_X];
    m_coreGraphsVP[VP_Y] = m_rightGraphWidgetVP[VP_Y];
    m_coreGraphsVP[VP_WIDTH] = m_rightGraphWidgetVP[VP_WIDTH];
    m_coreGraphsVP[VP_HEIGHT] = m_rightGraphWidgetVP[VP_HEIGHT]/2;
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

void Renderer::drawMainWidget() const {
    glViewport(m_mainWidgetVP[VP_X], m_mainWidgetVP[VP_Y],
               m_mainWidgetVP[VP_WIDTH], m_mainWidgetVP[VP_HEIGHT]);
}

void Renderer::drawRightGraphWidget() const {
    glViewport(m_rightGraphWidgetVP[VP_X], m_rightGraphWidgetVP[VP_Y],
               m_rightGraphWidgetVP[VP_WIDTH], m_rightGraphWidgetVP[VP_HEIGHT]);

    drawCoreGraphs();
}

void Renderer::drawCoreGraphs() const {
    glViewport(m_coreGraphsVP[VP_X], m_coreGraphsVP[VP_Y],
               m_coreGraphsVP[VP_WIDTH], m_coreGraphsVP[VP_HEIGHT]);

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
        glViewport(m_coreGraphsVP[VP_X],
                   (m_coreGraphsVP[VP_Y] + (numCores-1)*m_coreGraphsVP[VP_HEIGHT]/numCores)
                   - i*m_coreGraphsVP[VP_HEIGHT]/(numCores),
                   m_coreGraphsVP[VP_WIDTH],
                   m_coreGraphsVP[VP_HEIGHT]/numCores);

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

        drawGraphGrid(m_graphGridVertsID, m_graphGridIndicesID, m_graphIndicesSize);

        // Draw a label for the core graph
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
        char str[7] = { 'C', 'o', 'r', 'e', ' ', static_cast<char>(i) + '0', '\0'};
        m_fontManager.renderLine(-0.93f, 0.80f, RG_FONT_SMALL, str, sizeof(str));
    }
}

void Renderer::drawLeftGraphWidget() const {
    glViewport(m_leftGraphWidgetVP[VP_X], m_leftGraphWidgetVP[VP_Y],
               m_leftGraphWidgetVP[VP_WIDTH], m_leftGraphWidgetVP[VP_HEIGHT]);

    drawCpuGraph();
    drawRamGraph();
    drawGpuGraph();
    drawNetGraph();
}

void Renderer::drawCpuGraph() const {
    { // Graph grid/border
        glViewport(m_cpuGraphVP[VP_X], m_cpuGraphVP[VP_Y],
                   m_cpuGraphVP[VP_WIDTH], m_cpuGraphVP[VP_HEIGHT]);

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
    glViewport(m_cpuGraphVP[VP_X], m_cpuGraphVP[VP_Y],
               (m_cpuGraphVP[VP_WIDTH] * 4)/5, m_cpuGraphVP[VP_HEIGHT]);

    drawGraphGrid(m_graphGridVertsID, m_graphGridIndicesID, m_graphIndicesSize);
    glLineWidth(0.5f);
    drawLineGraph(m_cpuMeasure->getUsageData());

    { // Text
        glViewport(m_cpuGraphVP[VP_X] + (4 * m_cpuGraphVP[VP_WIDTH]) / 5, m_cpuGraphVP[VP_Y],
                   m_cpuGraphVP[VP_WIDTH] / 5, m_cpuGraphVP[VP_HEIGHT]);
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

        const char* str{ "Sys Load" };
        const char* min{ "0%" };
        const char* max{ "100%" };
        m_fontManager.renderLine(RG_FONT_SMALL, min,
                                 0, 0,
                                 m_cpuGraphVP[VP_WIDTH]/5, m_cpuGraphVP[VP_HEIGHT],
                                 RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
        m_fontManager.renderLine(RG_FONT_SMALL, str,
                                 0, 0,
                                 m_cpuGraphVP[VP_WIDTH]/5, m_cpuGraphVP[VP_HEIGHT],
                                 RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
        m_fontManager.renderLine(RG_FONT_SMALL, max,
                                 0, 0,
                                 m_cpuGraphVP[VP_WIDTH]/5, m_cpuGraphVP[VP_HEIGHT],
                                 RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
    }
}

void Renderer::drawRamGraph() const {
    glViewport(m_ramGraphVP[VP_X], m_ramGraphVP[VP_Y],
               m_ramGraphVP[VP_WIDTH] , m_ramGraphVP[VP_HEIGHT]);
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
    glViewport(m_ramGraphVP[VP_X], m_ramGraphVP[VP_Y],
               (m_ramGraphVP[VP_WIDTH]*4)/5 , m_ramGraphVP[VP_HEIGHT]);

    // Draw the background grid for the graph
    drawGraphGrid(m_graphGridVertsID, m_graphGridIndicesID, m_graphIndicesSize);

    drawLineGraph(m_ramMeasure->getUsageData());

    {// Set viewport for text drawing
        glViewport(m_ramGraphVP[VP_X] + (4 * m_ramGraphVP[VP_WIDTH]) / 5, m_ramGraphVP[VP_Y],
                   m_ramGraphVP[VP_WIDTH] / 5, m_ramGraphVP[VP_HEIGHT]);
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

        const char* str{ "RAM Load" };
        const char* min{ "0%" };
        const char* max{ "100%" };
        m_fontManager.renderLine(RG_FONT_SMALL, min,
                                 0, 0,
                                 m_ramGraphVP[VP_WIDTH]/5, m_ramGraphVP[VP_HEIGHT],
                                 RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
        m_fontManager.renderLine(RG_FONT_SMALL, str,
                                 0, 0,
                                 m_ramGraphVP[VP_WIDTH]/5, m_ramGraphVP[VP_HEIGHT],
                                 RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
        m_fontManager.renderLine(RG_FONT_SMALL, max,
                                 0, 0,
                                 m_ramGraphVP[VP_WIDTH]/5, m_ramGraphVP[VP_HEIGHT],
                                 RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
    }
}

void Renderer::drawGpuGraph() const {
    glViewport(m_gpuGraphVP[VP_X], m_gpuGraphVP[VP_Y],
               m_gpuGraphVP[VP_WIDTH] , m_gpuGraphVP[VP_HEIGHT]);
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
    glViewport(m_gpuGraphVP[VP_X], m_gpuGraphVP[VP_Y],
               (m_gpuGraphVP[VP_WIDTH]*4)/5 , m_gpuGraphVP[VP_HEIGHT]);

    // Draw the background grid for the graph
    drawGraphGrid(m_graphGridVertsID, m_graphGridIndicesID, m_graphIndicesSize);

    // Draw the line graph
    glLineWidth(0.5f);
    drawLineGraph(m_gpuMeasure->getUsageData());

    {// Set viewport for text drawing
        glViewport(m_gpuGraphVP[VP_X] + (4 * m_gpuGraphVP[VP_WIDTH]) / 5, m_gpuGraphVP[VP_Y],
                   m_gpuGraphVP[VP_WIDTH] / 5, m_gpuGraphVP[VP_HEIGHT]);
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

        const char* str{ "GPU Load" };
        const char* min{ "0%" };
        const char* max{ "100%" };
        m_fontManager.renderLine(RG_FONT_SMALL, min,
                                 0, 0,
                                 m_gpuGraphVP[VP_WIDTH]/5, m_gpuGraphVP[VP_HEIGHT],
                                 RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
        m_fontManager.renderLine(RG_FONT_SMALL, str,
                                 0, 0,
                                 m_gpuGraphVP[VP_WIDTH]/5, m_gpuGraphVP[VP_HEIGHT],
                                 RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
        m_fontManager.renderLine(RG_FONT_SMALL, max,
                                 0, 0,
                                 m_gpuGraphVP[VP_WIDTH]/5, m_gpuGraphVP[VP_HEIGHT],
                                 RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
    }

}

void Renderer::drawNetGraph() const {
    glViewport(m_netGraphVP[VP_X], m_netGraphVP[VP_Y],
               m_netGraphVP[VP_WIDTH] , m_netGraphVP[VP_HEIGHT]);
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
    glViewport(m_netGraphVP[VP_X], m_netGraphVP[VP_Y],
               (m_netGraphVP[VP_WIDTH]*4)/5 , m_netGraphVP[VP_HEIGHT]);

    // Draw the background grid for the graph
    drawGraphGrid(m_graphGridVertsID, m_graphGridIndicesID, m_graphIndicesSize);

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
        glViewport(m_netGraphVP[VP_X] + (4 * m_netGraphVP[VP_WIDTH]) / 5, m_netGraphVP[VP_Y],
                   m_netGraphVP[VP_WIDTH] / 5, m_netGraphVP[VP_HEIGHT]);
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

        const auto maxVal{ m_netMeasure->getMaxDownValue() };

        std::string suffix{ "B" };
        if (maxVal > 1000) {
            suffix = "KB";
        }
        if (maxVal > 1000 * 1000) {
            suffix = "MB";
        }


        /* Print the maximum throughput as the scale at the top of the graph */
        if (suffix == "MB") {
            char buff[8];
            snprintf(buff, sizeof(buff), "%5.1fMB", maxVal/static_cast<float>(MB));
            m_fontManager.renderLine(RG_FONT_SMALL, buff, 0, 0,
                                     m_gpuGraphVP[VP_WIDTH]/5, m_gpuGraphVP[VP_HEIGHT],
                                     RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
        } else if (suffix == "KB") {
            char buff[8];
            snprintf(buff, sizeof(buff), "%5.1fKB", maxVal/static_cast<float>(KB));
            m_fontManager.renderLine(RG_FONT_SMALL, buff, 0, 0,
                                     m_gpuGraphVP[VP_WIDTH]/5, m_gpuGraphVP[VP_HEIGHT],
                                     RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
        } else {
            const auto top{ std::to_string(maxVal) + suffix };
            char buff[5];
            snprintf(buff, sizeof(buff), "%3lluB", maxVal);
            m_fontManager.renderLine(RG_FONT_SMALL, buff, 0, 0,
                                     m_gpuGraphVP[VP_WIDTH]/5, m_gpuGraphVP[VP_HEIGHT],
                                     RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
        }

        const char* str{ "Down/Up" };
        const auto bottom = std::string{ "0" + suffix };
        m_fontManager.renderLine(RG_FONT_SMALL, bottom.c_str(), 0, 0,
                                 m_gpuGraphVP[VP_WIDTH]/5, m_gpuGraphVP[VP_HEIGHT],
                                 RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
        m_fontManager.renderLine(RG_FONT_SMALL, str, 0, 0,
                                 m_gpuGraphVP[VP_WIDTH]/5, m_gpuGraphVP[VP_HEIGHT],
                                 RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
    }
}

void Renderer::drawProcessWidget() const {
    glViewport(m_procWidgetVP[VP_X], m_procWidgetVP[VP_Y],
               m_procWidgetVP[VP_WIDTH], m_procWidgetVP[VP_HEIGHT]);
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

    auto procNames = std::vector<std::string>{  };
    auto procPercentages = std::vector<std::string>{  };
    procNames.reserve(m_processMeasure->getProcCPUData().size());
    procPercentages.reserve(m_processMeasure->getProcCPUData().size());
    for (const auto& pair : m_processMeasure->getProcCPUData()) {
        procNames.emplace_back(pair.first);

        // Convert percentage to string format
        char buff[6];
        snprintf(buff, sizeof(buff), "%4.1f%%", pair.second);
        procPercentages.emplace_back(buff);
    }

    m_fontManager.renderLines(RG_FONT_STANDARD, procNames, 0, 0,
                              m_procWidgetVP[VP_WIDTH]/2, m_procWidgetVP[VP_HEIGHT],
                              RG_ALIGN_LEFT | RG_ALIGN_CENTERED_VERTICAL, 15, 5);
    m_fontManager.renderLines(RG_FONT_STANDARD, procPercentages, 0, 0,
                              m_procWidgetVP[VP_WIDTH]/2, m_procWidgetVP[VP_HEIGHT],
                              RG_ALIGN_RIGHT | RG_ALIGN_CENTERED_VERTICAL, 15, 5);
}

void Renderer::drawProcRAMList() const {
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    auto procNames = std::vector<std::string>{  };
    auto procRamUsages = std::vector<std::string>{  };

    procNames.reserve(m_processMeasure->getProcRAMData().size());
    procRamUsages.reserve(m_processMeasure->getProcRAMData().size());
    for (const auto& pair : m_processMeasure->getProcRAMData()) {
        procNames.emplace_back(pair.first);

        // Convert RAM value to string format, assuming top RAM usages are only
        // ever in megabytes or gigabytes
        char buff[6];
        if (pair.second >= 1000) {
            snprintf(buff, sizeof(buff), "%.1fGB", pair.second / 1024.0f);
        } else {
            snprintf(buff, sizeof(buff), "%dMB", pair.second);
        }
        procRamUsages.emplace_back(buff);
    }

    m_fontManager.renderLines(RG_FONT_STANDARD, procNames, m_procWidgetVP[VP_WIDTH]/2, 0,
                              m_procWidgetVP[VP_WIDTH]/2, m_procWidgetVP[VP_HEIGHT],
                              RG_ALIGN_LEFT | RG_ALIGN_CENTERED_VERTICAL, 15, 5);
    m_fontManager.renderLines(RG_FONT_STANDARD, procRamUsages, m_procWidgetVP[VP_WIDTH]/2, 0,
                              m_procWidgetVP[VP_WIDTH]/2, m_procWidgetVP[VP_HEIGHT],
                              RG_ALIGN_RIGHT | RG_ALIGN_CENTERED_VERTICAL, 15, 5);

}

void Renderer::drawStatsWidget() const {
    glViewport(m_statsWidgetVP[VP_X], m_statsWidgetVP[VP_Y],
               m_statsWidgetVP[VP_WIDTH], m_statsWidgetVP[VP_HEIGHT]  );

    // Draw dividers
    glColor4f(DIVIDER_R, DIVIDER_G, DIVIDER_B, DIVIDER_A);
    glLineWidth(0.5f);
    glBegin(GL_LINES); {
        glVertex2f(-1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f); // Top

        glVertex2f(-1.0f, -1.0f);
        glVertex2f( 1.0f, -1.0f); // Bottom
    } glEnd();

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    m_fontManager.renderLines(RG_FONT_STANDARD, m_statsStrings, 0, 0,
                              m_statsWidgetVP[VP_WIDTH], m_statsWidgetVP[VP_HEIGHT],
                              RG_ALIGN_LEFT | RG_ALIGN_CENTERED_VERTICAL, 15, 10);
}

void Renderer::drawHDDWidget() const {
    glViewport(m_hddWidgetVP[VP_X], m_hddWidgetVP[VP_Y],
               m_hddWidgetVP[VP_WIDTH], m_hddWidgetVP[VP_HEIGHT]);
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
    glViewport(m_timeWidgetVP[VP_X], m_timeWidgetVP[VP_Y],
               m_timeWidgetVP[VP_WIDTH], m_timeWidgetVP[VP_HEIGHT]);
    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);
    glLineWidth(0.5f);

    constexpr float leftDivX{ -0.33f };
    constexpr float rightDivX{ 0.33f };
    constexpr float midDivY{ -0.3f };

    // Draw dividers
    {
        glBegin(GL_LINES); {
            glVertex2f(-1.0f, 1.0f);
            glVertex2f(1.0f, 1.0f); // Top line

            glVertex2f(-1.0f, -1.0f);
            glVertex2f(1.0f, -1.0f); // Bottom line

            glVertex2f(-0.9f, midDivY);
            glVertex2f(0.9f, midDivY); // Mid-divider

            glVertex2f(leftDivX, -1.0f);
            glVertex2f(leftDivX, -0.3f); // Left vertical

            glVertex2f(rightDivX, -1.0f);
            glVertex2f(rightDivX, -0.3f); // Right vertical
        } glEnd();
    }

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    // Draw the big system time
    {
        time_t now = time(0);
        tm t;
        char buf[9];
        localtime_s(&t, &now);
        strftime(buf, sizeof(buf), "%T", &t);

        // Get font width in pixels for horizontal centering
        const auto midDivYPx{ vpCoordsToPixels(midDivY, m_timeWidgetVP[VP_HEIGHT]) };

        m_fontManager.renderLine(RG_FONT_TIME, buf, 0, midDivYPx,
                                 m_timeWidgetVP[VP_WIDTH], m_timeWidgetVP[VP_HEIGHT] - midDivYPx,
                                 RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_CENTERED_HORIZONTAL);

        // Draw the year and month in bottom-middle
        // TODO use full month names
        char dateBuf[12];
        strftime(dateBuf, sizeof(dateBuf), "%d %B", &t);
        m_fontManager.renderLine(RG_FONT_STANDARD, dateBuf,
                                 vpCoordsToPixels(leftDivX, m_timeWidgetVP[VP_WIDTH]), 0,
                                 m_timeWidgetVP[VP_WIDTH]/3,
                                 vpCoordsToPixels(midDivY, m_timeWidgetVP[VP_HEIGHT]),
                                 RG_ALIGN_BOTTOM | RG_ALIGN_CENTERED_HORIZONTAL, 10, 15);

        char yearBuf[5];
        strftime(yearBuf, sizeof(yearBuf), "%Y", &t);
        m_fontManager.renderLine(RG_FONT_STANDARD_BOLD, yearBuf,
                                 vpCoordsToPixels(leftDivX, m_timeWidgetVP[VP_WIDTH]), 0,
                                 m_timeWidgetVP[VP_WIDTH]/3,
                                 vpCoordsToPixels(midDivY, m_timeWidgetVP[VP_HEIGHT]),
                                 RG_ALIGN_TOP | RG_ALIGN_CENTERED_HORIZONTAL, 10, 15);
    }

    // Draw the uptime in bottom-left
    {
        // Spacing between text and the divider
        const int32_t dividerOffset{ 10 };
        const int32_t leftDivPixelPos{
            static_cast<int32_t>(((leftDivX + 1.0f) / 2.0f) * m_timeWidgetVP[VP_WIDTH]) };

        // Get the width of the string in pixels, then start drawing the string
        // from a position such that the last character is "dividerOffset" pixels
        // away from the divider
        const char* uptimeTitle{ "UPTIME" };
        int32_t strLenPixels{ m_fontManager.getFontCharWidth(RG_FONT_STANDARD) *
            static_cast<int32_t>(strlen(uptimeTitle)) };
        int32_t strStartPixelX{ leftDivPixelPos - strLenPixels - dividerOffset};

        // Convert pixel value to viewport coordinates
        float strStartVPX{ ((static_cast<float>(strStartPixelX) / m_timeWidgetVP[VP_WIDTH]) * 2.0f) - 1.0f};
        //m_fontManager.renderLine(strStartVPX, -0.55f, RG_FONT_STANDARD, uptimeTitle);
        m_fontManager.renderLine(RG_FONT_STANDARD_BOLD, uptimeTitle, 0, 0,
                                 m_timeWidgetVP[VP_WIDTH]/3,
                                 vpCoordsToPixels(midDivY, m_timeWidgetVP[VP_HEIGHT]),
                                 RG_ALIGN_RIGHT | RG_ALIGN_TOP, 10, 15);

        const auto& uptime{ m_cpuMeasure->getUptimeStr() };

        strLenPixels = static_cast<int32_t>(uptime.size()) * m_fontManager.getFontCharWidth(RG_FONT_STANDARD);
        strStartPixelX = leftDivPixelPos - strLenPixels - dividerOffset;
        strStartVPX =  ((static_cast<float>(strStartPixelX) / m_timeWidgetVP[VP_WIDTH]) * 2.0f) - 1.0f;

        glRasterPos2f(strStartVPX, -0.8f);
        glCallLists(uptime.length(), GL_UNSIGNED_BYTE, uptime.c_str());
    }

    // Draw network connection status in bottom-right
    {
        // Get region to render text in
        const auto renderX{ vpCoordsToPixels(rightDivX, m_timeWidgetVP[VP_WIDTH]) };
        const auto renderY{ 0 };
        const auto renderWidth{ m_timeWidgetVP[VP_WIDTH] - renderX };
        const auto renderHeight{ vpCoordsToPixels(midDivY, m_timeWidgetVP[VP_HEIGHT]) };

        m_fontManager.renderLine(RG_FONT_STANDARD_BOLD, "NETWORK", renderX,
                                 renderY, renderWidth, renderHeight,
                                 RG_ALIGN_LEFT | RG_ALIGN_TOP, 10, 15);

        if (m_netMeasure->isConnected()) {
            m_fontManager.renderLine(RG_FONT_STANDARD, "UP", renderX,
                                     renderY, renderWidth, renderHeight,
                                     RG_ALIGN_LEFT | RG_ALIGN_BOTTOM, 10, 15);
            // TODO print ping in ms
        } else {
            m_fontManager.renderLine(RG_FONT_STANDARD, "DOWN", renderX,
                                     renderY, renderWidth, renderHeight,
                                     RG_ALIGN_LEFT | RG_ALIGN_BOTTOM, 10, 15);
        }
    }
}

}