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
    m_graphWidgetViewport{ marginX, windowHeight/4 + 2*marginY,
                              windowWidth/5, windowHeight/2 }, // Left-Mid
    m_cpuGraphViewport{ m_graphWidgetViewport[0], m_graphWidgetViewport[1] + 3*m_graphWidgetViewport[3]/4,
                        m_graphWidgetViewport[2], m_graphWidgetViewport[3]/4}
{

}

Renderer::~Renderer() {
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

    // Create the different fonts
    HFONT standardFont = CreateFont(20, 10, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                              OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH | FF_DONTCARE, fonts[0]);
    HFONT standardFontBold = CreateFont(20, 10, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
                              OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH | FF_DONTCARE, fonts[0]);
    HFONT largeFont = CreateFont(70, 30, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                           OUT_RASTER_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                           VARIABLE_PITCH | FF_MODERN, fonts[0]);

    // Bind the fonts to the OpenGL display lists
    HDC hdc = GetDC(m_renderTargetHandle);
    SelectObject(hdc, standardFont);
    wglUseFontBitmaps(hdc, 0, 256, stdFontBase);
    SelectObject(hdc, standardFontBold);
    wglUseFontBitmaps(hdc, 0, 256, stdFontBoldBase);
    SelectObject(hdc, largeFont);
    wglUseFontBitmaps(hdc, 0, 256, lrgFontBase);

    // Cleanup the fonts
    DeleteObject(largeFont);
    DeleteObject(standardFont);
    DeleteObject(standardFontBold);

    // Set the default font
    glListBase(stdFontBase);
}

void Renderer::release() {
    glDeleteLists(stdFontBase, 96);
}

void Renderer::draw(const GLShaderHandler& shaders) const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, BGCOLOR_A);

    drawGraphWidget();
    drawProcessWidget();
    drawTimeWidget();
    drawHDDWidget();

    //m_cpuMeasure.draw(shaders.getCpuGraphProgram());
    //m_gpuMeasure.draw();
    //m_ramMeasure.draw();
    //m_processMeasure.draw();
    //m_driveMeasure.draw();
    //m_sysInfo.draw();
}

void Renderer::drawGraphWidget() const {
    glViewport(m_graphWidgetViewport[0], m_graphWidgetViewport[1],
               m_graphWidgetViewport[2], m_graphWidgetViewport[3]);
    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);

    drawCpuGraph();
}

void Renderer::drawCpuGraph() const {
    glViewport(m_cpuGraphViewport[0], m_cpuGraphViewport[1],
               m_cpuGraphViewport[2], m_cpuGraphViewport[3]);

    // Draw the graph border
    glLineWidth(0.5f);
    glColor4f(DIVIDER_R, DIVIDER_G, DIVIDER_B, DIVIDER_A);
    glBegin(GL_LINES); {
        glVertex2f(-1.0f, 1.0f);
        glVertex2f( 1.0f, 1.0f); // Top

        glVertex2f(-1.0f, -1.0f);
        glVertex2f( 1.0f, -1.0f); // Bottom

        glVertex2f(-1.0f,  1.0f);
        glVertex2f(-1.0f, -1.0f); // Left

        glVertex2f( 1.0f,  1.0f);
        glVertex2f( 1.0f, -1.0f); // Right
    } glEnd();

    // Draw the graph background grid
    constexpr float numRows{ 5.0f };
    constexpr float numCols{ 12.0f };

    glLineWidth(0.5f);
    glColor4f(DIVIDER_R, DIVIDER_G, DIVIDER_B, 0.2f);
    glBegin(GL_LINES); {
        for (auto i{ 0U }; i < numRows; ++i) {
            const auto y = float{ (i + 1)/(numRows + 1) * 2.0f - 1.0f };
            glVertex2f(-1.0f, y);
            glVertex2f( 1.0f, y);
        }
        for (auto i{ 0U }; i < numCols; ++i) {
            const auto x = float{ (i + 1)/(numCols + 1) * 2.0f - 1.0f };
            glVertex2f(x, -1.0f);
            glVertex2f(x,  1.0f);
        }
    } glEnd();



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

            // If the vertex is at the border, add/subtract the border delta
            /*if (i == 0) {
                x += bDelta;
            }*/

            glVertex3f(x, y, 0.0f);
        }
    } glEnd();
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

        // Draw the process's CPU usage
        glRasterPos2f(-0.13f, rasterY);
        char buff[5];
        snprintf(buff, sizeof(buff), "%.1f%%", pair.second);
        //const std::string usage{ std::to_string(static_cast<uint8_t>(pair.second)) + "%" };
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
        char buf[12];
        localtime_s(&t, &now);
        strftime(buf, sizeof(buf), "%X %p", &t);

        glRasterPos2f(-0.92f, 0.15f);
        fontScope(lrgFontBase, [&]() {
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