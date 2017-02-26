#include "Renderer.h"

#include <GL/gl.h>
#include <sstream>
#include <iostream>

#include "CPUMeasure.h"
#include "GPUMeasure.h"
#include "RAMMeasure.h"
#include "ProcessMeasure.h"
#include "DriveMeasure.h"
#include "SystemInfo.h"
#include "GLShaderHandler.h"
#include "colors.h"
#include "utils.h"

namespace rg {

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
                          windowWidth / 5, windowHeight / 6 } // Top left
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
    HDC hdc = GetDC(m_renderTargetHandle);

    /*LargeFont = CreateFont(40, 20, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                           OUT_RASTER_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                           VARIABLE_PITCH | FF_MODERN, fonts[0]);
    SelectObject(hdc, LargeFont);
    wglUseFontBitmaps(hdc, 0, 256, 1000);*/

    stdFontBase = glGenLists(256);
    StandardFont = CreateFont(20, 10, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                              OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH | FF_DONTCARE, fonts[0]);
    lrgFontBase = glGenLists(256);
    LargeFont = CreateFont(70, 30, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                           OUT_RASTER_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                           VARIABLE_PITCH | FF_MODERN, fonts[0]);

    SelectObject(hdc, StandardFont);
    wglUseFontBitmaps(hdc, 0, 256, stdFontBase);
    SelectObject(hdc, LargeFont);
    wglUseFontBitmaps(hdc, 0, 256, lrgFontBase);

    // Set the standard font to be default
    glListBase(stdFontBase);
}

void Renderer::release() {
    glDeleteLists(stdFontBase, 96);
}

void Renderer::draw(const GLShaderHandler& shaders) const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClearColor(BGCOLOR_R, BGCOLOR_G, BGCOLOR_B, BGCOLOR_A);

    drawTimeWidget();

    m_cpuMeasure.draw(shaders.getCpuGraphProgram());
    //m_gpuMeasure.draw();
    m_ramMeasure.draw();
    //m_processMeasure.draw();
    m_driveMeasure.draw();
    m_sysInfo.draw();
}

void Renderer::drawTimeWidget() const {
    glViewport(m_timeWidgetViewport[0], m_timeWidgetViewport[1],
               m_timeWidgetViewport[2], m_timeWidgetViewport[3]);
    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);
    glLineWidth(0.5f);

    //drawViewportBorder();

    // Draw Dividers
    glBegin(GL_LINES); {
        glVertex2f(-1.0f, 1.0f); // Drawing at the very edge of the viewport like this leaves an even thinner line
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

    // Draw the big system time
    {
        time_t now = time(0);
        tm t;
        char buf[12];
        localtime_s(&t, &now);
        strftime(buf, sizeof(buf), "%X %p", &t);

        glRasterPos2f(-0.92f, 0.15f);

        glPushAttrib(GL_LIST_BIT);
        glListBase(lrgFontBase);
        glCallLists(sizeof(buf), GL_UNSIGNED_BYTE, buf);
        glPopAttrib();
    }

    // Draw the uptime in bottom-left
    {

    }

}

}