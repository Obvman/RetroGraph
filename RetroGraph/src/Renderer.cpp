#include "../headers/Renderer.h"

#include <GL/gl.h>
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

// Activates the given font for the lifetime of the given function f only, then
// Returns to the previously selected font
template<typename F>
void fontScope(GLint fontBase, F f) {
    glPushAttrib(GL_LIST_BIT);
    glListBase(fontBase);
    f();
    glPopAttrib();
}

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

Renderer::Renderer()
{

}

Renderer::~Renderer() {
}

void Renderer::init(HWND hWnd, uint16_t windowWidth, uint16_t windowHeight,
                    const CPUMeasure& _cpu, const GPUMeasure& _gpu,
                    const RAMMeasure& _ram, const NetMeasure& _net,
                    const ProcessMeasure& _proc, const DriveMeasure& _drive,
                    const SystemInfo& _sys) {
    m_cpuMeasure = &_cpu;
    m_gpuMeasure = &_gpu;
    m_ramMeasure = &_ram;
    m_netMeasure = &_net;
    m_processMeasure = &_proc;
    m_driveMeasure = &_drive;
    m_sysInfo = &_sys;

    initViewportBuffers(windowWidth, windowHeight);

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

    initFonts(hWnd);
    initVBOs();
    initShaders();
}

void Renderer::initViewportBuffers(uint16_t windowWidth, uint16_t windowHeight) {
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

    m_graphWidgetVP[0] = marginX;
    m_graphWidgetVP[1] = windowHeight/4 + 2*marginY;
    m_graphWidgetVP[2] = windowWidth/5;
    m_graphWidgetVP[3] = windowHeight/2; // Left-Mid

    m_cpuGraphVP[0] = m_graphWidgetVP[0];
    m_cpuGraphVP[1] = m_graphWidgetVP[1] + 3*m_graphWidgetVP[3]/4;
    m_cpuGraphVP[2] = m_graphWidgetVP[2];
    m_cpuGraphVP[3] = m_graphWidgetVP[3]/4;

    m_ramGraphVP[0] = m_graphWidgetVP[0];
    m_ramGraphVP[1] = m_graphWidgetVP[1] + 2*m_graphWidgetVP[3]/4;
    m_ramGraphVP[2] = m_graphWidgetVP[2];
    m_ramGraphVP[3] = m_graphWidgetVP[3]/4;

    m_gpuGraphVP[0] = m_graphWidgetVP[0];
    m_gpuGraphVP[1] = m_graphWidgetVP[1] + 1*m_graphWidgetVP[3]/4;
    m_gpuGraphVP[2] = m_graphWidgetVP[2];
    m_gpuGraphVP[3] = m_graphWidgetVP[3]/4;

    m_netGraphVP[0] = m_graphWidgetVP[0];
    m_netGraphVP[1] = m_graphWidgetVP[1] + 0*m_graphWidgetVP[3]/4;
    m_netGraphVP[2] = m_graphWidgetVP[2];
    m_netGraphVP[3] = m_graphWidgetVP[3]/4;

    m_mainWidgetVP[0] = marginX + windowWidth/2 - windowWidth/5;
    m_mainWidgetVP[1] = 2*marginY + windowHeight/4;
    m_mainWidgetVP[2] = 2 * windowWidth/5;
    m_mainWidgetVP[3] = windowHeight/2; // Midd;

    m_coreGraphsVP[0] = m_mainWidgetVP[0];
    m_coreGraphsVP[1] = m_mainWidgetVP[1];
    m_coreGraphsVP[2] = m_mainWidgetVP[2];
    m_coreGraphsVP[3] = m_mainWidgetVP[3]/2;
}

void Renderer::initFonts(HWND hWnd) {
    m_renderTargetHandle = hWnd;

    // Array for conveniently testing different fonts
    const char* const fonts[] = {
        "Courier New",
        "Lato Lights",
        "Orator Std"
        "Verdana"
        //"OCR A Extended"
        "Letter Gothic Std",
        "Kozuka Gothic Pr6N L",
        "Algerian",
    };

    // Generate the display lists for OpenGL to quickly draw the fonts
    stdFontBase = glGenLists(256);
    stdFontBoldBase = glGenLists(256);
    lrgFontBase = glGenLists(256);
    smlFontBase = glGenLists(256);
    timeFontBase = glGenLists(256);

    RECT rect;
    GetWindowRect(m_renderTargetHandle, &rect);
    const auto width{ rect.right - rect.left };
    const auto height{ rect.bottom - rect.top };

    const auto stdFontHeight{ std::lround(height / 100.0) - 2 };
    const auto stdFontWidth{ std::lround(width / 100.0) - 2};

    // Create the different fonts
    HFONT standardFont = CreateFont(
        stdFontWidth, stdFontHeight, 0, 0, FW_NORMAL,
        FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, fonts[0]);
    HFONT standardFontBold = CreateFont(
        stdFontWidth, stdFontHeight, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, fonts[0]);
    HFONT largeFont = CreateFont(
        70, 45, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_RASTER_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        VARIABLE_PITCH | FF_MODERN, fonts[0]);
    HFONT smallFont = CreateFont(
        stdFontWidth/2 + 2, stdFontHeight/2 + 2, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_RASTER_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        VARIABLE_PITCH | FF_MODERN, fonts[1]);
    HFONT timeFont = CreateFont(
        stdFontWidth * 4, stdFontHeight * 4, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_RASTER_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        VARIABLE_PITCH | FF_MODERN, fonts[1]);


    // Bind the fonts to the OpenGL display lists
    HDC hdc = GetDC(m_renderTargetHandle);
    SelectObject(hdc, standardFont);
    wglUseFontBitmaps(hdc, 0, 256, stdFontBase);
    SelectObject(hdc, standardFontBold);
    wglUseFontBitmaps(hdc, 0, 256, stdFontBoldBase);
    SelectObject(hdc, largeFont);
    wglUseFontBitmaps(hdc, 0, 256, lrgFontBase);
    SelectObject(hdc, smallFont);
    wglUseFontBitmaps(hdc, 0, 256, smlFontBase);
    SelectObject(hdc, timeFont);
    wglUseFontBitmaps(hdc, 0, 256, timeFontBase);

    // Cleanup the fonts
    DeleteObject(timeFont);
    DeleteObject(smallFont);
    DeleteObject(largeFont);
    DeleteObject(standardFont);
    DeleteObject(standardFontBold);

    // Set the default font
    glListBase(stdFontBase);
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

void Renderer::release() {
    // Free VBO memory
    glDeleteBuffers(1, &m_graphGridVertsID);
    glDeleteBuffers(1, &m_graphGridIndicesID);

    // Delete font display lists from OpenGL memory
    glDeleteLists(stdFontBase, 256);
    glDeleteLists(stdFontBoldBase, 256);
    glDeleteLists(lrgFontBase, 256);
    glDeleteLists(smlFontBase, 256);
}

void Renderer::draw(uint32_t ticks) const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, BGCOLOR_A);

    drawGraphWidget();

    drawProcessWidget();
    drawTimeWidget();
    drawHDDWidget();
    drawStatsWidget();

    drawMainWidget();
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
    //drawViewportBorder();

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
    for (auto i = size_t{ 0U }; i < numCores; ++i) {
        // Set the viewport for the current graph. If i is even, the graph
        // sits on the left, otherwise it's on the right. The y position
        // of each graph changes as we draw more
        glColor4f(GRAPHLINE_A, GRAPHLINE_G, GRAPHLINE_B, GRAPHLINE_A);
        if (i % 2 == 0) {
            glViewport(m_coreGraphsVP[0],
                       m_coreGraphsVP[1] + i*m_coreGraphsVP[3]/(2*numRows),
                       m_coreGraphsVP[2]/numRows,
                       m_coreGraphsVP[3]/numRows);
        } else {
            glViewport(m_coreGraphsVP[0] + m_coreGraphsVP[2]/numRows,
                       m_coreGraphsVP[1] + (i-1)*m_coreGraphsVP[3]/(2*numRows),
                       m_coreGraphsVP[2]/numRows,
                       m_coreGraphsVP[3]/numRows);
        }

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
        fontScope(smlFontBase, [i]() {
            glRasterPos2f(-0.93f, 0.80f);
            char str[7] = { 'C', 'o', 'r', 'e', ' ', static_cast<char>(i) + '0', '\0'};
            glCallLists(strlen(str), GL_UNSIGNED_BYTE, str);
        });
    }
}

void Renderer::drawGraphWidget() const {
    glViewport(m_graphWidgetVP[0], m_graphWidgetVP[1],
               m_graphWidgetVP[2], m_graphWidgetVP[3]);

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
    drawFilledGraph(m_cpuMeasure->getUsageData());

    { // Text
        glViewport(m_cpuGraphVP[0] + (4 * m_cpuGraphVP[2]) / 5, m_cpuGraphVP[1],
                   m_cpuGraphVP[2] / 5, m_cpuGraphVP[3]);
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

        fontScope(smlFontBase, []() {
            glRasterPos2f(-0.8f, -0.02f);
            const char* str{ "Sys Load" };
            glCallLists(strlen(str), GL_UNSIGNED_BYTE, str);

            glRasterPos2f(-0.8f, -0.77f);
            const char* min{ "0%" };
            glCallLists(strlen(min), GL_UNSIGNED_BYTE, min);

            glRasterPos2f(-0.8f, 0.70f);
            const char* max{ "100%" };
            glCallLists(strlen(max), GL_UNSIGNED_BYTE, max);
        });
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

        fontScope(smlFontBase, []() {
            glRasterPos2f(-0.8f, -0.02f);
            const char* str{ "RAM Load" };
            glCallLists(strlen(str), GL_UNSIGNED_BYTE, str);

            glRasterPos2f(-0.8f, -0.77f);
            const char* min{ "0%" };
            glCallLists(strlen(min), GL_UNSIGNED_BYTE, min);

            glRasterPos2f(-0.8f, 0.70f);
            const char* max{ "100%" };
            glCallLists(strlen(max), GL_UNSIGNED_BYTE, max);
        });
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
    drawFilledGraph(m_gpuMeasure->getUsageData());

    {// Set viewport for text drawing
        glViewport(m_gpuGraphVP[0] + (4 * m_gpuGraphVP[2]) / 5, m_gpuGraphVP[1],
                   m_gpuGraphVP[2] / 5, m_gpuGraphVP[3]);
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

        fontScope(smlFontBase, []() {
            glRasterPos2f(-0.8f, -0.02f);
            const char* str{ "GPU Load" };
            glCallLists(strlen(str), GL_UNSIGNED_BYTE, str);

            glRasterPos2f(-0.8f, -0.77f);
            const char* min{ "0%" };
            glCallLists(strlen(min), GL_UNSIGNED_BYTE, min);

            glRasterPos2f(-0.8f, 0.70f);
            const char* max{ "100%" };
            glCallLists(strlen(max), GL_UNSIGNED_BYTE, max);
        });
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
        glBegin(GL_QUADS); {
            glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.7f);
            for (auto i = size_t{ 0U }; i < downData.size() - 1; ++i) {
                const auto percent1 = float{ (downData[i] / static_cast<float>(MB)) / maxValMB };
                const auto percent2 = float{ (downData[i+1] / static_cast<float>(MB)) / maxValMB };

                const auto x1 = float{ (static_cast<float>(i) / (downData.size() - 1)) * 2.0f - 1.0f };
                const auto y1 = float{ percent1 * 2.0f - 1.0f };
                const auto x2 = float{ (static_cast<float>(i+1) / (downData.size() - 1)) * 2.0f - 1.0f };
                const auto y2 = float{ percent2 * 2.0f - 1.0f };

                glVertex2f(x1, -1.0f); // Bottom-left
                glVertex2f(x1, y1);    // Top-left
                glVertex2f(x2, y2);    // Top-right
                glVertex2f(x2, -1.0f); // Bottom-right
            }
        } glEnd();

        // Draw the upload graph
        glBegin(GL_QUADS); {
            glColor4f(PINK1_R, PINK1_G, PINK1_B, 0.7f);
            for (auto i = size_t{ 0U }; i < upData.size() - 1; ++i) {
                const auto percent1 = float{ (upData[i] / static_cast<float>(MB)) / maxValMB };
                const auto percent2 = float{ (upData[i+1] / static_cast<float>(MB)) / maxValMB };

                const auto x1 = float{ (static_cast<float>(i) / (upData.size() - 1)) * 2.0f - 1.0f };
                const auto y1 = float{ percent1 * 2.0f - 1.0f };
                const auto x2 = float{ (static_cast<float>(i+1) / (upData.size() - 1)) * 2.0f - 1.0f };
                const auto y2 = float{ percent2 * 2.0f - 1.0f };

                glVertex2f(x1, -1.0f); // Bottom-left
                glVertex2f(x1, y1); // Top-left
                glVertex2f(x2, y2); // Top-right
                glVertex2f(x2, -1.0f); // Bottom-right
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

        fontScope(smlFontBase, [maxVal, &suffix]() {
            glRasterPos2f(-0.8f, -0.02f);
            const char* str{ "Down/Up" };
            glCallLists(strlen(str), GL_UNSIGNED_BYTE, str);

            glRasterPos2f(-0.8f, -0.77f);
            const auto bottom{ "0" + suffix };
            glCallLists(strlen(bottom.c_str()), GL_UNSIGNED_BYTE, bottom.c_str());

            glRasterPos2f(-0.8f, 0.70f);
            if (suffix == "MB") {
                char buff[8];
                snprintf(buff, sizeof(buff), "%5.1fMB", maxVal/static_cast<float>(MB));
                glCallLists(sizeof(buff), GL_UNSIGNED_BYTE, buff);
            } else if (suffix == "KB") {
                char buff[8];
                snprintf(buff, sizeof(buff), "%5.1fKB", maxVal/static_cast<float>(KB));
                glCallLists(sizeof(buff), GL_UNSIGNED_BYTE, buff);
            } else {
                const auto top{ std::to_string(maxVal) + suffix };
                char buff[5];
                snprintf(buff, sizeof(buff), "%3lluB", maxVal);
                glCallLists(strlen(top.c_str()), GL_UNSIGNED_BYTE, top.c_str());
            }
        });
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

    {// Draw dividers
        glBegin(GL_LINES); {
            glVertex2f(-1.0f, 1.0f);
            glVertex2f(1.0f, 1.0f); // Top line

            glVertex2f(-1.0f, -1.0f);
            glVertex2f(1.0f, -1.0f); // Bottom line

            glVertex2f(-0.9f, -0.3f);
            glVertex2f(0.9f, -0.3f); // Mid-divider

            glVertex2f(-0.33f, -1.0f);
            glVertex2f(-0.33f, -0.3f); // Left vertical

            glVertex2f(0.33f, -1.0f);
            glVertex2f(0.33f, -0.3f); // Right vertical
        } glEnd();
    }

    // Draw all the text elements
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    { // Draw the big system time
        time_t now = time(0);
        tm t;
        char buf[9];
        localtime_s(&t, &now);
        strftime(buf, sizeof(buf), "%T", &t);

        glRasterPos2f(-0.95f, 0.15f);
        fontScope(timeFontBase, [&]() {
            glCallLists(sizeof(buf), GL_UNSIGNED_BYTE, buf);
        });

        // Draw the date in the bottom-middle
        char dateBuf[7];
        strftime(dateBuf, sizeof(dateBuf), "%d %b", &t);
        glRasterPos2f(-0.15f, -0.8f);
        glCallLists(sizeof(dateBuf), GL_UNSIGNED_BYTE, dateBuf);

        char yearBuf[5];
        strftime(yearBuf, sizeof(yearBuf), "%Y", &t);

        glRasterPos2f(-0.1f, -0.55f);
        fontScope(stdFontBoldBase, [&]() {
            glCallLists(sizeof(yearBuf), GL_UNSIGNED_BYTE, yearBuf);
        });
    }

    // Draw the uptime in bottom-left
    {
        glRasterPos2f(-0.70f, -0.55f);
        fontScope(stdFontBoldBase, [&]() {
            glCallLists(6, GL_UNSIGNED_BYTE, "UPTIME");
        });

        const auto& uptime{ m_cpuMeasure->getUptimeStr() };
        glRasterPos2f(-0.90f, -0.8f);
        glCallLists(uptime.length(), GL_UNSIGNED_BYTE, uptime.c_str());
    }

    // TODO draw something else useful in the bottom-right
    {
        glRasterPos2f(0.4f, -0.55f);

        fontScope(stdFontBoldBase, [&]() {
            glCallLists(5, GL_UNSIGNED_BYTE, "TEMP");
        });

        glRasterPos2f(0.4f, -0.8f);
        glCallLists(10, GL_UNSIGNED_BYTE, "Temporary");
    }
}

}