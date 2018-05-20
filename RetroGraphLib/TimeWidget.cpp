#include "stdafx.h"

#include "TimeWidget.h"

// #include <GL/glew.h>
// #include <GL/gl.h>
#include <Windows.h>

#include "drawUtils.h"
#include "colors.h"
#include "FontManager.h"
#include "CPUMeasure.h"
#include "NetMeasure.h"
#include "RetroGraph.h"

namespace rg {

void TimeWidget::updateObservers(const RetroGraph & rg) {
    m_cpuMeasure = rg.getCPUMeasure().get();
    m_netMeasure = rg.getNetMeasure().get();
}

void TimeWidget::draw() const {
    if (!m_visible) return;

    clear();

    drawWidgetBackground();

    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);
    glLineWidth(0.5f);

    constexpr float leftDivX{ -0.33f };
    constexpr float rightDivX{ 0.33f };
    constexpr float midDivY{ -0.3f };
    const auto midDivYPx{ vpCoordsToPixels(midDivY, m_viewport.height) };

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
    // Draw the big system time and the date below
    {
        time_t now = time(0);
        tm t;
        char timeBuff[10];
        localtime_s(&t, &now);
        strftime(timeBuff, sizeof(timeBuff), "%T", &t);

        // Get font width in pixels for horizontal centering
        m_fontManager->renderLine(RG_FONT_TIME, timeBuff, 0, midDivYPx,
                                  m_viewport.width, m_viewport.height -
                                  midDivYPx, RG_ALIGN_CENTERED_VERTICAL |
                                  RG_ALIGN_CENTERED_HORIZONTAL);

        // Draw the year and month and day in bottom-middle
        char dateBuff[12];
        strftime(dateBuff, sizeof(dateBuff), "%d %B", &t);
        m_fontManager->renderLine(RG_FONT_STANDARD, dateBuff,
                                  vpCoordsToPixels(leftDivX, m_viewport.width),
                                  0, m_viewport.width/3, midDivYPx,
                                  RG_ALIGN_BOTTOM |
                                  RG_ALIGN_CENTERED_HORIZONTAL, 10, 15);

        char dayBuff[10];
        strftime(dayBuff, sizeof(dayBuff), "%A", &t);
        m_fontManager->renderLine(RG_FONT_STANDARD_BOLD, dayBuff,
                                  vpCoordsToPixels(leftDivX, m_viewport.width),
                                  0, m_viewport.width/3, midDivYPx,
                                  RG_ALIGN_TOP | RG_ALIGN_CENTERED_HORIZONTAL,
                                  10, 15);
    }

    // Draw the uptime in bottom-left
    m_fontManager->renderLine(RG_FONT_STANDARD_BOLD, "Uptime", 0, 0,
                              m_viewport.width/3, midDivYPx, RG_ALIGN_RIGHT |
                              RG_ALIGN_TOP, 10, 15);
    m_fontManager->renderLine(RG_FONT_STANDARD,
                              m_cpuMeasure->getUptimeStr().c_str(), 0, 0,
                              m_viewport.width/3, midDivYPx, RG_ALIGN_RIGHT |
                              RG_ALIGN_BOTTOM, 10, 15);

    // Draw network connection status in bottom-right
    {
        // Get region to render text in
        const auto renderX{ vpCoordsToPixels(rightDivX, m_viewport.width) };
        const auto renderY{ 0 };
        const auto renderWidth{ m_viewport.width - renderX };
        const auto renderHeight{ vpCoordsToPixels(midDivY, m_viewport.height) };

        m_fontManager->renderLine(RG_FONT_STANDARD_BOLD, "Network", renderX,
                                 renderY, renderWidth, renderHeight,
                                 RG_ALIGN_LEFT | RG_ALIGN_TOP, 10, 15);

        if (m_netMeasure->isConnected()) {
            m_fontManager->renderLine(RG_FONT_STANDARD, "UP", renderX,
                                     renderY, renderWidth, renderHeight,
                                     RG_ALIGN_LEFT | RG_ALIGN_BOTTOM, 10, 15);
        } else {
            m_fontManager->renderLine(RG_FONT_STANDARD, "DOWN", renderX,
                                     renderY, renderWidth, renderHeight,
                                     RG_ALIGN_LEFT | RG_ALIGN_BOTTOM, 10, 15);
        }
    }

};

}
