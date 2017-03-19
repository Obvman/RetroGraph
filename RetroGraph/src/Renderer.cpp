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
    initViewports(windowWidth, windowHeight);
    initVBOs();
    initShaders();
}

void Renderer::draw(uint32_t) const {
    //glScissor(m_leftGraphWidgetVP.x, m_leftGraphWidgetVP.y,
               //m_leftGraphWidgetVP.width, m_leftGraphWidgetVP.height);
    //glEnable(GL_SCISSOR_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, BGCOLOR_A);
    //glDisable(GL_SCISSOR_TEST);

    drawLeftGraphWidget();

    drawProcessWidget();
    drawTimeWidget();
    drawHDDWidget();

    drawLeftStatsWidget();
    drawRightStatsWidget();

    drawRightGraphWidget();
}

void Renderer::release() {
    // Free VBO memory
    glDeleteBuffers(1, &m_graphGridVertsID);
    glDeleteBuffers(1, &m_graphGridIndicesID);

    m_fontManager.release();
}

/********************* Private Functions ********************/
void Renderer::initViewports(uint32_t windowWidth, uint32_t windowHeight) {
    const auto widgetW{ windowWidth/5 };
    const auto widgetH{ windowHeight/6 };
    const auto sideWidgetH{ windowHeight/2 };

    // Top left
    m_timeVP.set(marginX, windowHeight - marginY - widgetH,
                 widgetW, widgetH);

    // Top right
    m_hddVP.set(windowWidth - widgetW - marginX, windowHeight - marginY - widgetH,
                widgetW, widgetH);

    // Bottom middle, twice as wide as regular widgets
    m_procVP.set(marginX + windowWidth/2 - widgetW, marginY,
                 2*widgetW, widgetH);

    // Bottom left
    m_leftStatsVP.set(marginX, marginY, widgetW, widgetH);

    // Bottom right
    m_rightStatsVP.set(windowWidth - marginX - widgetW, marginY, 
                       widgetW, widgetH);

    // Mid left
    m_leftGraphWidgetVP.set(marginX, windowHeight/2 - windowHeight/4,
                            widgetW, windowHeight/2);
    // Each of these graphs are one after the other vertically
    m_cpuGraphVP.set(m_leftGraphWidgetVP.x,
                     m_leftGraphWidgetVP.y + 3*m_leftGraphWidgetVP.height/4,
                     m_leftGraphWidgetVP.width,
                     m_leftGraphWidgetVP.height/4);

    m_ramGraphVP.set(m_leftGraphWidgetVP.x,
                     m_leftGraphWidgetVP.y + 2*m_leftGraphWidgetVP.height/4,
                     m_leftGraphWidgetVP.width,
                     m_leftGraphWidgetVP.height/4);

    m_gpuGraphVP.set(m_leftGraphWidgetVP.x,
                     m_leftGraphWidgetVP.y + 1*m_leftGraphWidgetVP.height/4,
                     m_leftGraphWidgetVP.width,
                     m_leftGraphWidgetVP.height/4);

    m_netGraphVP.set(m_leftGraphWidgetVP.x,
                     m_leftGraphWidgetVP.y + 0*m_leftGraphWidgetVP.height/4,
                     m_leftGraphWidgetVP.width,
                     m_leftGraphWidgetVP.height/4);

    // Middle
    m_mainWidgetVP.set(marginX + sideWidgetH - widgetW,
                       2*marginY + windowHeight/4,
                       2 * widgetW,
                       sideWidgetH);

    // Mid right
    m_rightGraphWidgetVP.set(windowWidth - widgetW - marginX, 
                             windowHeight/2 - windowHeight/4,
                             widgetW, 
                             sideWidgetH);

    m_coreGraphsVP.set(m_rightGraphWidgetVP.x,
                       m_rightGraphWidgetVP.y,
                       m_rightGraphWidgetVP.width,
                       m_rightGraphWidgetVP.height/2 - 5);

    m_rightCPUStatsVP.set(m_rightGraphWidgetVP.x,
                          m_rightGraphWidgetVP.y + m_rightGraphWidgetVP.height/2 + 5,
                          m_rightGraphWidgetVP.width,
                          m_rightGraphWidgetVP.height/2);
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
    glViewport(m_mainWidgetVP.x, m_mainWidgetVP.y,
               m_mainWidgetVP.width, m_mainWidgetVP.height);
}

void Renderer::drawRightGraphWidget() const {
    glViewport(m_rightGraphWidgetVP.x, m_rightGraphWidgetVP.y,
               m_rightGraphWidgetVP.width, m_rightGraphWidgetVP.height);

    drawCoreGraphs();
    drawCPUStatsWidget();
}

void Renderer::drawCoreGraphs() const {
    glViewport(m_coreGraphsVP.x, m_coreGraphsVP.y,
               m_coreGraphsVP.width, m_coreGraphsVP.height);

    // Draw x rows of core graphs, with 2 graphs per row until all graphs
    // are drawn
    const auto numCores{ m_cpuMeasure->getPerCoreUsageData().size() };
    const auto numRows{ numCores / 2 }; // Fair to assume we have an even number of cores

    glLineWidth(0.5f);
    glColor4f(GRAPHLINE_A, GRAPHLINE_G, GRAPHLINE_B, GRAPHLINE_A);
    for (auto i = size_t{ 0U }; i < numCores; ++i) {
        // Set the viewport for the current graph. The y position
        // of each graph changes as we draw more
        glViewport(m_coreGraphsVP.x,
                   (m_coreGraphsVP.y + (numCores-1)*m_coreGraphsVP.height/numCores)
                   - i*m_coreGraphsVP.height/(numCores),
                   m_coreGraphsVP.width,
                   m_coreGraphsVP.height/numCores);

        drawLineGraph(m_cpuMeasure->getPerCoreUsageData()[i]);

        // Draw the border for this core graph
        drawBorder();
        drawGraphGrid(m_graphGridVertsID, m_graphGridIndicesID, m_graphIndicesSize);

        // Draw a label for the core graph
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
        char str[7];
        snprintf(str, sizeof(str), "Core %d", i);
        m_fontManager.renderLine(RG_FONT_SMALL, str, 0, 0, 0, 0,
                                 RG_ALIGN_TOP | RG_ALIGN_LEFT, 10, 10);
    }
}

void Renderer::drawCPUStatsWidget() const {
    glViewport(m_rightCPUStatsVP.x, m_rightCPUStatsVP.y,
               m_rightCPUStatsVP.width, m_rightCPUStatsVP.height);

    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);
    drawTopSerifLine(-1.0f, 1.0f);
    drawBottomSerifLine(-1.0f, 1.0f);

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    const auto fontHeight{ m_fontManager.getFontCharHeight(RG_FONT_STANDARD) };
    constexpr auto bottomTextMargin{ 10U };

    char voltBuff[7];
    char clockBuff[12];
    snprintf(voltBuff, sizeof(voltBuff), "%.3fv", m_cpuMeasure->getVoltage());
    snprintf(clockBuff, sizeof(clockBuff), "%7.3fMHz", m_cpuMeasure->getClockSpeed());

    m_fontManager.renderLine(RG_FONT_STANDARD, voltBuff, 0, 0, 0, 0,
            RG_ALIGN_LEFT | RG_ALIGN_BOTTOM, bottomTextMargin, bottomTextMargin);
    m_fontManager.renderLine(RG_FONT_STANDARD, clockBuff, 0, 0, 0, 0,
            RG_ALIGN_RIGHT | RG_ALIGN_BOTTOM, bottomTextMargin, bottomTextMargin);

    // Draw the temperature bar for each CPU core
    const auto numCores{ m_cpuMeasure->getNumCores() };
    for (auto i = size_t{ 0U }; i < numCores; ++i) {

        glViewport(m_rightCPUStatsVP.x + i * m_rightCPUStatsVP.width/numCores,
                   // Leave space for text below the bars
                   m_rightCPUStatsVP.y + fontHeight + bottomTextMargin,
                   m_rightCPUStatsVP.width / numCores,
                   m_rightCPUStatsVP.height - fontHeight - bottomTextMargin);

        constexpr auto maxTemp = float{ 100.0f };
        constexpr auto barWidth = float{ 0.3f };
        constexpr auto bottomY = float{ -0.7f };
        constexpr auto topY = float{ 0.7f };
        constexpr auto rangeY { topY - bottomY };
        constexpr auto barStartX{ ((2.0f - barWidth) / 2.0f) - 1.0f };
        const auto percentage{ m_cpuMeasure->getTemp(i) / maxTemp };

        // Draw the Core temperature bar
        glBegin(GL_QUADS); {
            glColor3f(BARFILLED_R, BARFILLED_G, BARFILLED_B);
            glVertex2f(barStartX, bottomY);
            glVertex2f(barStartX,  bottomY + percentage * rangeY);
            glVertex2f(barStartX + barWidth, bottomY + percentage * rangeY);
            glVertex2f(barStartX + barWidth, bottomY);

            glColor3f(BARFREE_R, BARFREE_G, BARFREE_B);
            glVertex2f(barStartX, bottomY + percentage * rangeY);
            glVertex2f(barStartX,  topY);
            glVertex2f(barStartX + barWidth,  topY);
            glVertex2f(barStartX + barWidth, bottomY + percentage * rangeY);
        } glEnd();

        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
        char coreBuff[3] = { 'C', '0' + static_cast<char>(i), '\0' };
        m_fontManager.renderLine(RG_FONT_STANDARD, coreBuff, 0, 0, 0, 0,
                                 RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_BOTTOM,
                                 10, 10);
        char tempBuff[6];
        snprintf(tempBuff, sizeof(tempBuff), "%.0fC", m_cpuMeasure->getTemp(i));
        m_fontManager.renderLine(RG_FONT_STANDARD, tempBuff, 0, 0, 0, 0,
                                 RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_TOP,
                                 10, 10);
    }
}

void Renderer::drawLeftGraphWidget() const {
    glViewport(m_leftGraphWidgetVP.x, m_leftGraphWidgetVP.y,
               m_leftGraphWidgetVP.width, m_leftGraphWidgetVP.height);

    drawCpuGraph();
    drawRamGraph();
    drawGpuGraph();
    drawNetGraph();
}

void Renderer::drawCpuGraph() const {
    glViewport(m_cpuGraphVP.x, m_cpuGraphVP.y,
               m_cpuGraphVP.width, m_cpuGraphVP.height);
    drawBorder();

    // Set the viewport for the graph to be left section
    glViewport(m_cpuGraphVP.x, m_cpuGraphVP.y,
               (m_cpuGraphVP.width * 4)/5, m_cpuGraphVP.height);

    drawGraphGrid(m_graphGridVertsID, m_graphGridIndicesID, m_graphIndicesSize);
    glLineWidth(0.5f);
    drawLineGraph(m_cpuMeasure->getUsageData());

    // Text
    glViewport(m_cpuGraphVP.x + (4 * m_cpuGraphVP.width) / 5, m_cpuGraphVP.y,
               m_cpuGraphVP.width / 5, m_cpuGraphVP.height);
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    m_fontManager.renderLine(RG_FONT_SMALL, "0%",
                             0, 0,
                             m_cpuGraphVP.width/5, m_cpuGraphVP.height,
                             RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
    m_fontManager.renderLine(RG_FONT_SMALL, "CPU Load",
                             0, 0,
                             m_cpuGraphVP.width/5, m_cpuGraphVP.height,
                             RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
    m_fontManager.renderLine(RG_FONT_SMALL, "100%",
                             0, 0,
                             m_cpuGraphVP.width/5, m_cpuGraphVP.height,
                             RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
}

void Renderer::drawRamGraph() const {
    glViewport(m_ramGraphVP.x, m_ramGraphVP.y,
               m_ramGraphVP.width , m_ramGraphVP.height);
    drawBorder();

    // Set the viewport for the graph itself to be left section
    glViewport(m_ramGraphVP.x, m_ramGraphVP.y,
               (m_ramGraphVP.width*4)/5 , m_ramGraphVP.height);

    // Draw the background grid for the graph
    drawGraphGrid(m_graphGridVertsID, m_graphGridIndicesID, m_graphIndicesSize);

    drawLineGraph(m_ramMeasure->getUsageData());

    // Set viewport for text drawing
    glViewport(m_ramGraphVP.x + (4 * m_ramGraphVP.width) / 5, m_ramGraphVP.y,
               m_ramGraphVP.width / 5, m_ramGraphVP.height);
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    m_fontManager.renderLine(RG_FONT_SMALL, "0%",
                             0, 0,
                             m_ramGraphVP.width/5, m_ramGraphVP.height,
                             RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
    m_fontManager.renderLine(RG_FONT_SMALL, "RAM Load",
                             0, 0,
                             m_ramGraphVP.width/5, m_ramGraphVP.height,
                             RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
    m_fontManager.renderLine(RG_FONT_SMALL, "100%",
                             0, 0,
                             m_ramGraphVP.width/5, m_ramGraphVP.height,
                             RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);

}

void Renderer::drawGpuGraph() const {
    glViewport(m_gpuGraphVP.x, m_gpuGraphVP.y,
               m_gpuGraphVP.width , m_gpuGraphVP.height);
    drawBorder();

    // Set the viewport for the graph to be left section
    glViewport(m_gpuGraphVP.x, m_gpuGraphVP.y,
               (m_gpuGraphVP.width*4)/5 , m_gpuGraphVP.height);
    drawGraphGrid(m_graphGridVertsID, m_graphGridIndicesID, m_graphIndicesSize);
    glLineWidth(0.5f);
    drawLineGraph(m_gpuMeasure->getUsageData());

    // Set viewport for text drawing
    glViewport(m_gpuGraphVP.x + (4 * m_gpuGraphVP.width) / 5, m_gpuGraphVP.y,
               m_gpuGraphVP.width / 5, m_gpuGraphVP.height);
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    m_fontManager.renderLine(RG_FONT_SMALL, "0%",
                             0, 0,
                             m_gpuGraphVP.width/5, m_gpuGraphVP.height,
                             RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
    m_fontManager.renderLine(RG_FONT_SMALL, "GPU Load",
                             0, 0,
                             m_gpuGraphVP.width/5, m_gpuGraphVP.height,
                             RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
    m_fontManager.renderLine(RG_FONT_SMALL, "100%",
                             0, 0,
                             m_gpuGraphVP.width/5, m_gpuGraphVP.height,
                             RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
}

void Renderer::drawNetGraph() const {
    glViewport(m_netGraphVP.x, m_netGraphVP.y,
               m_netGraphVP.width , m_netGraphVP.height);
    drawBorder();

    // Set the viewport for the graph to be left section
    glViewport(m_netGraphVP.x, m_netGraphVP.y,
               (m_netGraphVP.width*4)/5 , m_netGraphVP.height);
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
        glViewport(m_netGraphVP.x + (4 * m_netGraphVP.width) / 5, m_netGraphVP.y,
                   m_netGraphVP.width / 5, m_netGraphVP.height);
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

        const auto maxVal{ m_netMeasure->getMaxDownValue() };
        std::string suffix{ "B" };
        if (maxVal > 1000 * 1000) {
            suffix = "MB";
        } else if (maxVal > 1000) {
            suffix = "KB";
        }

        /* Print the maximum throughput as the scale at the top of the graph */
        if (suffix == "MB") {
            char buff[8];
            snprintf(buff, sizeof(buff), "%5.1fMB", maxVal/static_cast<float>(MB));
            m_fontManager.renderLine(RG_FONT_SMALL, buff, 0, 0,
                                     m_gpuGraphVP.width/5, m_gpuGraphVP.height,
                                     RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
        } else if (suffix == "KB") {
            char buff[8];
            snprintf(buff, sizeof(buff), "%5.1fKB", maxVal/static_cast<float>(KB));
            m_fontManager.renderLine(RG_FONT_SMALL, buff, 0, 0,
                                     m_gpuGraphVP.width/5, m_gpuGraphVP.height,
                                     RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
        } else {
            const auto top{ std::to_string(maxVal) + suffix };
            char buff[5];
            snprintf(buff, sizeof(buff), "%3lluB", maxVal);
            m_fontManager.renderLine(RG_FONT_SMALL, buff, 0, 0,
                                     m_gpuGraphVP.width/5, m_gpuGraphVP.height,
                                     RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
        }

        const auto bottom = std::string{ "0" + suffix };
        m_fontManager.renderLine(RG_FONT_SMALL, bottom.c_str(), 0, 0,
                                 m_gpuGraphVP.width/5, m_gpuGraphVP.height,
                                 RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
        m_fontManager.renderLine(RG_FONT_SMALL, "Down / Up", 0, 0,
                                 m_gpuGraphVP.width/5, m_gpuGraphVP.height,
                                 RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
    }
}

void Renderer::drawProcessWidget() const {
    glViewport(m_procVP.x, m_procVP.y,
               m_procVP.width, m_procVP.height);

    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);
    glLineWidth(0.5f);
    drawTopSerifLine(-1.0f, 1.0f);
    drawBottomSerifLine(-1.0f, 1.0f);
    glBegin(GL_LINES); {
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
                              m_procVP.width/2, m_procVP.height,
                              RG_ALIGN_LEFT | RG_ALIGN_CENTERED_VERTICAL, 15, 5);
    m_fontManager.renderLines(RG_FONT_STANDARD, procPercentages, 0, 0,
                              m_procVP.width/2, m_procVP.height,
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

    m_fontManager.renderLines(RG_FONT_STANDARD, procNames, m_procVP.width/2, 0,
                              m_procVP.width/2, m_procVP.height,
                              RG_ALIGN_LEFT | RG_ALIGN_CENTERED_VERTICAL, 15, 5);
    m_fontManager.renderLines(RG_FONT_STANDARD, procRamUsages, m_procVP.width/2, 0,
                              m_procVP.width/2, m_procVP.height,
                              RG_ALIGN_RIGHT | RG_ALIGN_CENTERED_VERTICAL, 15, 5);

}

void Renderer::drawLeftStatsWidget() const {
    glViewport(m_leftStatsVP.x, m_leftStatsVP.y, m_leftStatsVP.width, m_leftStatsVP.height);

    glColor4f(DIVIDER_R, DIVIDER_G, DIVIDER_B, DIVIDER_A);
    glLineWidth(0.5f);
    drawTopSerifLine(-1.0f, 1.0f);
    drawBottomSerifLine(-1.0f, 1.0f);

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    m_fontManager.renderLines(RG_FONT_STANDARD, m_statsStrings, 0, 0,
                              m_leftStatsVP.width, m_leftStatsVP.height,
                              RG_ALIGN_LEFT | RG_ALIGN_CENTERED_VERTICAL, 15, 10);
}

void Renderer::drawRightStatsWidget() const {
    glViewport(m_rightStatsVP.x, m_rightStatsVP.y,
               m_rightStatsVP.width, m_rightStatsVP.height);
    drawViewportBorder();
}

void Renderer::drawHDDWidget() const {
    glViewport(m_hddVP.x, m_hddVP.y,
               m_hddVP.width, m_hddVP.height);

    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);
    glLineWidth(0.5f);
    drawTopSerifLine(-1.0f, 1.0f);
    drawBottomSerifLine(-1.0f, 1.0f);

    // Draw each drive status section
    const auto& drives{ m_driveMeasure->getDrives() };
    for (auto i = size_t{ 0 }; i < drives.size(); ++i) {
        glViewport(m_hddVP.x + i * (m_hddVP.width/drives.size()), m_hddVP.y,
                   m_hddVP.width/drives.size(), m_hddVP.height);

        // Draw the drive label on the bottom
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
        const char label[3]{ drives[i]->getDriveLetter(), ':', '\0' };
        m_fontManager.renderLine(RG_FONT_STANDARD, label, 0, 0, 0, 0,
                                 RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_BOTTOM,
                                 0, 10);

        // Draw the capacity up top
        m_fontManager.renderLine(RG_FONT_STANDARD,
                                 drives[i]->getCapacityStr().c_str(),
                                 0, 0, 0, 0,
                                 RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_TOP,
                                 0, 10);

        const auto percentage{ static_cast<float>(drives[i]->getUsedBytes()) /
                               drives[i]->getTotalBytes() };
        constexpr auto barWidth = float{ 0.3f };
        constexpr auto bottomY = float{ -0.5f };
        constexpr auto topY = float{ 0.5f };
        constexpr auto rangeY{ topY - bottomY };
        const float barStartX{ ((2.0f - barWidth) / 2.0f) - 1.0f};
        glBegin(GL_QUADS); {
            glColor3f(BARFILLED_R, BARFILLED_G, BARFILLED_B);
            glVertex2f(barStartX, bottomY);
            glVertex2f(barStartX,  bottomY + percentage * rangeY);
            glVertex2f(barStartX + barWidth, bottomY + percentage * rangeY);
            glVertex2f(barStartX + barWidth, bottomY);

            glColor3f(BARFREE_R, BARFREE_G, BARFREE_B);
            glVertex2f(barStartX, bottomY + percentage * rangeY);
            glVertex2f(barStartX,  topY);
            glVertex2f(barStartX + barWidth,  topY);
            glVertex2f(barStartX + barWidth, bottomY + percentage * rangeY);
        } glEnd();
    }
}

void Renderer::drawTimeWidget() const {
    glViewport(m_timeVP.x, m_timeVP.y,
               m_timeVP.width, m_timeVP.height);
    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);
    glLineWidth(0.5f);

    constexpr float leftDivX{ -0.33f };
    constexpr float rightDivX{ 0.33f };
    constexpr float midDivY{ -0.3f };

    // Draw dividers
    drawTopSerifLine(-1.0f, 1.0f);
    drawBottomSerifLine(-1.0f, 1.0f);
    drawSerifLine(-0.9f, 0.9f, midDivY);

    glBegin(GL_LINES); {
        glVertex2f(leftDivX, -1.0f);
        glVertex2f(leftDivX, -0.3f); // Left vertical

        glVertex2f(rightDivX, -1.0f);
        glVertex2f(rightDivX, -0.3f); // Right vertical
    } glEnd();

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    // Draw the big system time
    {
        time_t now = time(0);
        tm t;
        char buf[9];
        localtime_s(&t, &now);
        strftime(buf, sizeof(buf), "%T", &t);

        // Get font width in pixels for horizontal centering
        const auto midDivYPx{ vpCoordsToPixels(midDivY, m_timeVP.height) };

        m_fontManager.renderLine(RG_FONT_TIME, buf, 0, midDivYPx,
                                 m_timeVP.width, m_timeVP.height - midDivYPx,
                                 RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_CENTERED_HORIZONTAL);

        // Draw the year and month in bottom-middle
        // TODO use full month names
        char dateBuf[12];
        strftime(dateBuf, sizeof(dateBuf), "%d %B", &t);
        m_fontManager.renderLine(RG_FONT_STANDARD, dateBuf,
                                 vpCoordsToPixels(leftDivX, m_timeVP.width), 0,
                                 m_timeVP.width/3,
                                 vpCoordsToPixels(midDivY, m_timeVP.height),
                                 RG_ALIGN_BOTTOM | RG_ALIGN_CENTERED_HORIZONTAL, 10, 15);

        char yearBuf[5];
        strftime(yearBuf, sizeof(yearBuf), "%Y", &t);
        m_fontManager.renderLine(RG_FONT_STANDARD_BOLD, yearBuf,
                                 vpCoordsToPixels(leftDivX, m_timeVP.width), 0,
                                 m_timeVP.width/3,
                                 vpCoordsToPixels(midDivY, m_timeVP.height),
                                 RG_ALIGN_TOP | RG_ALIGN_CENTERED_HORIZONTAL, 10, 15);
    }

    // Draw the uptime in bottom-left
    {
        // Convert pixel value to viewport coordinates
        m_fontManager.renderLine(RG_FONT_STANDARD_BOLD, "UPTIME",
                                 0, 0,
                                 m_timeVP.width/3,
                                 vpCoordsToPixels(midDivY, m_timeVP.height),
                                 RG_ALIGN_RIGHT | RG_ALIGN_TOP, 10, 15);

        m_fontManager.renderLine(RG_FONT_STANDARD, m_cpuMeasure->getUptimeStr().c_str(),
                                 0, 0,
                                 m_timeVP.width/3,
                                 vpCoordsToPixels(midDivY, m_timeVP.height),
                                 RG_ALIGN_RIGHT | RG_ALIGN_BOTTOM, 10, 15);
    }

    // Draw network connection status in bottom-right
    {
        // Get region to render text in
        const auto renderX{ vpCoordsToPixels(rightDivX, m_timeVP.width) };
        const auto renderY{ 0 };
        const auto renderWidth{ m_timeVP.width - renderX };
        const auto renderHeight{ vpCoordsToPixels(midDivY, m_timeVP.height) };

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
