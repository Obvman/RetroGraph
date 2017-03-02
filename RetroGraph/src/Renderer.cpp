#include "../headers/Renderer.h"

#include <GL/gl.h>
#include <sstream>
#include <iostream>

#include "../headers/CPUMeasure.h"
#include "../headers/GPUMeasure.h"
#include "../headers/RAMMeasure.h"
#include "../headers/ProcessMeasure.h"
#include "../headers/DriveMeasure.h"
#include "../headers/SystemInfo.h"
#include "../headers/GLShaderHandler.h"
#include "../headers/colors.h"
#include "../headers/utils.h"

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


Renderer::Renderer(const CPUMeasure& _cpu, const GPUMeasure& _gpu,
                   const RAMMeasure& _ram, const ProcessMeasure& _proc,
                   const DriveMeasure& _drive, const SystemInfo& _sys,
                   uint16_t windowWidth, uint16_t windowHeight) :
    m_cpuMeasure{ _cpu },
    m_gpuMeasure{ _gpu },
    m_ramMeasure{ _ram },
    m_processMeasure{ _proc },
    m_driveMeasure{ _drive },
    m_sysInfo{ _sys },
    m_timeWidgetViewport{ marginX, 5 * windowHeight/6 - marginY,
                          windowWidth/5, windowHeight/6 }, // Top left
    m_hddWidgetViewport{ windowWidth - windowWidth/5 - marginX, 5 * windowHeight/6 - marginY,
                         windowWidth/5, windowHeight/6}, // Top right
    m_procWidgetViewport{ windowWidth/2 - windowWidth/5, marginY,
                          2*windowWidth/5, windowHeight/6}, // Bottom middle
    m_statsWidgetViewport{ marginX, marginY,
                           windowWidth/5, windowHeight/6 }, // Bottom left
    m_graphWidgetViewport{ marginX, windowHeight/4 + 2*marginY,
                              windowWidth/5, windowHeight/2 }, // Left-Mid
    m_cpuGraphViewport{ m_graphWidgetViewport[0], m_graphWidgetViewport[1] + 3*m_graphWidgetViewport[3]/4,
                        m_graphWidgetViewport[2], m_graphWidgetViewport[3]/4},
    m_ramGraphViewport{ m_graphWidgetViewport[0], m_graphWidgetViewport[1] + 2*m_graphWidgetViewport[3]/4,
                        m_graphWidgetViewport[2], m_graphWidgetViewport[3]/4}
{

}

Renderer::~Renderer() {
}

void Renderer::init(HWND hWnd) {
    initFonts(hWnd);
    initVBOs();
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
        constexpr size_t numVertLines{ 14U };
        constexpr size_t numHorizLines{ 7U };

        std::vector<GLfloat> gVerts;
        std::vector<GLuint> gIndices;
        gVerts.reserve(4 * (numVertLines + numHorizLines) );
        gIndices.reserve(2 * (numVertLines + numHorizLines) );

        // Fill the vertex and index arrays with data for drawing grid as VBO
        for (auto i{ 0U }; i < numVertLines; ++i) {
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
        for (auto i{ 0U }; i < numHorizLines; ++i) {
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

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

void Renderer::draw(const GLShaderHandler& shaders) const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, BGCOLOR_A);

    drawGraphWidget();

    drawProcessWidget();
    drawTimeWidget();
    drawHDDWidget();
    drawStatsWidget();

}

void Renderer::drawGraphWidget() const {
    glViewport(m_graphWidgetViewport[0], m_graphWidgetViewport[1],
               m_graphWidgetViewport[2], m_graphWidgetViewport[3]);
    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);

    drawCpuGraph();
    drawRamGraph();
}

void Renderer::drawCpuGraph() const {
    glViewport(m_cpuGraphViewport[0], m_cpuGraphViewport[1],
               m_cpuGraphViewport[2] , m_cpuGraphViewport[3]);
    // Draw border
    glLineWidth(0.5f);
    glColor4f(BORDER_R, BORDER_G, BORDER_B, BORDER_A);
    glBegin(GL_LINES); {
        glVertex2f(-1.0f, 1.0f); // Top
        glVertex2f(1.0f, 1.0f);

        glVertex2f(-1.0f, -1.0f); // Bottom
        glVertex2f(1.0f, -1.0f);

        glVertex2f(-1.0f, 1.0f); // Left
        glVertex2f(-1.0f, -1.0f);

        glVertex2f(1.0f, 1.0f); // Right
        glVertex2f(1.0f, -1.0f);
    } glEnd();

    // Set the viewport for the graph to be left section
    glViewport(m_cpuGraphViewport[0], m_cpuGraphViewport[1],
               (m_cpuGraphViewport[2]*4)/5 , m_cpuGraphViewport[3]);

    // Draw the background grid for the graph
    vboDrawScope(m_graphGridVertsID, m_graphGridIndicesID, [this]() {
        glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.2f);
        glLineWidth(0.5f);
        glDrawElements(GL_LINES, m_graphIndicesSize, GL_UNSIGNED_INT, 0);
    });

    // Draw the line graph
    glLineWidth(0.5f);
    glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, GRAPHLINE_A);
    // TODO use VBOs instead
    const auto& data{ m_cpuMeasure.getUsageData() };
    glBegin(GL_LINE_STRIP); {
        // Draw each node in the graph
        for (auto i{ 0U }; i < data.size(); ++i) {
            glColor4f(LINE_R, LINE_G, LINE_B, 1.0f);

            auto x = float{ (static_cast<float>(i) / (data.size() - 1)) * 2.0f - 1.0f };
            const auto y = float{ data[i] * 2.0f - 1.0f };

            glVertex3f(x, y, 0.0f);
        }
    } glEnd();

    // Set viewport for text drawing
    glViewport(m_cpuGraphViewport[0] + (4*m_cpuGraphViewport[2])/5, m_cpuGraphViewport[1],
               m_cpuGraphViewport[2]/5 , m_cpuGraphViewport[3]);
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

void Renderer::drawRamGraph() const {
    glViewport(m_ramGraphViewport[0], m_ramGraphViewport[1],
               m_ramGraphViewport[2] , m_ramGraphViewport[3]);
    // Draw border
    glLineWidth(0.5f);
    glColor4f(BORDER_R, BORDER_G, BORDER_B, BORDER_A);
    glBegin(GL_LINES); {
        glVertex2f(-1.0f, 1.0f); // Top
        glVertex2f(1.0f, 1.0f);

        glVertex2f(-1.0f, -1.0f); // Bottom
        glVertex2f(1.0f, -1.0f);

        glVertex2f(-1.0f, 1.0f); // Left
        glVertex2f(-1.0f, -1.0f);

        glVertex2f(1.0f, 1.0f); // Right
        glVertex2f(1.0f, -1.0f);
    } glEnd();

    // Set the viewport for the graph to be left section
    glViewport(m_ramGraphViewport[0], m_ramGraphViewport[1],
               (m_ramGraphViewport[2]*4)/5 , m_ramGraphViewport[3]);

    // Draw the background grid for the graph
    vboDrawScope(m_graphGridVertsID, m_graphGridIndicesID, [this]() {
        glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.2f);
        glLineWidth(0.5f);
        glDrawElements(GL_LINES, m_graphIndicesSize, GL_UNSIGNED_INT, 0);
    });

    // Draw the line graph
    glLineWidth(0.5f);
    glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, GRAPHLINE_A);
    // TODO use VBOs instead
    const auto& data{ m_ramMeasure.getUsageData() };
    glBegin(GL_LINE_STRIP); {
        // Draw each node in the graph
        for (auto i{ 0U }; i < data.size(); ++i) {
            glColor4f(LINE_R, LINE_G, LINE_B, 1.0f);

            auto x = float{ (static_cast<float>(i) / (data.size() - 1)) * 2.0f - 1.0f };
            const auto y = float{ data[i] * 2.0f - 1.0f };

            glVertex3f(x, y, 0.0f);
        }
    } glEnd();

    // Set viewport for text drawing
    glViewport(m_ramGraphViewport[0] + (4*m_ramGraphViewport[2])/5, m_ramGraphViewport[1],
               m_ramGraphViewport[2]/5 , m_ramGraphViewport[3]);
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

void Renderer::drawProcessWidget() const {
    glViewport(m_procWidgetViewport[0], m_procWidgetViewport[1],
               m_procWidgetViewport[2], m_procWidgetViewport[3]);
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
    auto rasterY = float{ 0.74f }; // Y changes for each process drawn

    for (const auto& pair : m_processMeasure.getProcCPUData()) {
        // Draw the process name
        glRasterPos2f(rasterX, rasterY);
        glCallLists(pair.first.length(), GL_UNSIGNED_BYTE, pair.first.c_str());

        // Draw the process's CPU percentage
        glRasterPos2f(-0.13f, rasterY);
        char buff[5];
        snprintf(buff, sizeof(buff), "%.1f%%", pair.second);
        glCallLists(sizeof(buff), GL_UNSIGNED_BYTE, buff);

        rasterY -= 1.8f / (m_processMeasure.getProcCPUData().size());
    }
}

void Renderer::drawProcRAMList() const {
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    const auto rasterX = float{ 0.05f };
    auto rasterY = float{ 0.74f }; // Y changes for each process drawn

    for (const auto& pair : m_processMeasure.getProcRAMData()) {
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

        rasterY -= 1.8f / (m_processMeasure.getProcCPUData().size());
    }

}

void Renderer::drawStatsWidget() const {
    glViewport(m_statsWidgetViewport[0], m_statsWidgetViewport[1],
               m_statsWidgetViewport[2], m_statsWidgetViewport[3]  );

    // Draw dividers
    glColor4f(DIVIDER_R, DIVIDER_G, DIVIDER_B, DIVIDER_A);
    glLineWidth(0.5f);
    glBegin(GL_LINES); {
        glVertex2f(-1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f); // Top

        glVertex2f(-1.0f, -1.0f);
        glVertex2f( 1.0f, -1.0f); // Bottom
    } glEnd();

    constexpr auto numLinesToDraw{ 6U };
    const auto rasterX = float{ -0.95f };
    auto rasterY = float{ 0.80f };
    const auto yRange{ 2.0f };

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    // Draw username@Computername, unix style
    {
        const auto& str{ m_sysInfo.getUserName() + "@" + m_sysInfo.getComputerName() };
        glRasterPos2f(rasterX, rasterY);
        glCallLists(str.length(), GL_UNSIGNED_BYTE, str.c_str());
        rasterY -= yRange / numLinesToDraw;
    }
    // Draw Windows version
    {
        const auto& str{ m_sysInfo.getOSInfoStr() };
        glRasterPos2f(rasterX, rasterY);
        glCallLists(str.size(), GL_UNSIGNED_BYTE, str.c_str());
        rasterY -= yRange / numLinesToDraw;
    }
    // Draw CPU name
    {
        const auto str = std::string{ m_cpuMeasure.getCPUName() };
        glRasterPos2f(rasterX, rasterY);
        glCallLists(str.length(), GL_UNSIGNED_BYTE, str.c_str());
        rasterY -= yRange / numLinesToDraw;
    }
    // Draw GPU Name
    {
        const auto& str{ m_sysInfo.getGPUDescription() };
        glRasterPos2f(rasterX, rasterY);
        glCallLists(str.length(), GL_UNSIGNED_BYTE, str.c_str());
        rasterY -= yRange / numLinesToDraw;
    }
    // Draw RAM capacity
    {
        const auto& str{ m_sysInfo.getRAMDescription() };
        glRasterPos2f(rasterX, rasterY);
        glCallLists(str.length(), GL_UNSIGNED_BYTE, str.c_str());
        rasterY -= yRange / numLinesToDraw;
    }
    // Draw GPU Temperature
    {
        const auto& str{ "GPU Temp: " + std::to_string(m_gpuMeasure.getCurrentTemp()) + "C" };
        glRasterPos2f(rasterX, rasterY);
        glCallLists(str.length(), GL_UNSIGNED_BYTE, str.c_str());
        rasterY -= yRange / numLinesToDraw;
    }
}

void Renderer::drawHDDWidget() const {
    glViewport(m_hddWidgetViewport[0], m_hddWidgetViewport[1],
               m_hddWidgetViewport[2], m_hddWidgetViewport[3]);
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
    const auto numDrives{ m_driveMeasure.getNumDrives() };
    // For each drive, draw a bar and label in the widget
    for (const auto& pdi : m_driveMeasure.getDrives()) {
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
    glViewport(m_timeWidgetViewport[0], m_timeWidgetViewport[1],
               m_timeWidgetViewport[2], m_timeWidgetViewport[3]);
    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);
    glLineWidth(0.5f);

    // Draw dividers
    glBegin(GL_LINES); {
        glVertex2f(-1.0f, 1.0f);
        glVertex2f( 1.0f, 1.0f); // Top line

        glVertex2f(-1.0f, -1.0f);
        glVertex2f( 1.0f, -1.0f); // Bottom line

        glVertex2f(-0.9f, -0.3f);
        glVertex2f( 0.9f, -0.3f); // Mid-divider

        glVertex2f(-0.33f, -1.0f);
        glVertex2f(-0.33f, -0.3f); // Left vertical

        glVertex2f(0.33f, -1.0f);
        glVertex2f(0.33f, -0.3f); // Right vertical
    } glEnd();

    // Draw all the text elements
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    // Draw the big system time
    {
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

        const auto& uptime{ m_cpuMeasure.getUptimeStr() };
        glRasterPos2f(-0.95f, -0.8f);
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