#include "../headers/CPUStatsWidget.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <Windows.h>

#include "../headers/utils.h"
#include "../headers/colors.h"
#include "../headers/FontManager.h"
#include "../headers/CPUMeasure.h"

namespace rg {

void CPUStatsWidget::setViewport(Viewport vp) { 
    m_viewport = vp; 
    m_coreGraphViewport = Viewport{ m_viewport.x, m_viewport.y,
        m_viewport.width, m_viewport.height/2 };
    m_statsViewport = Viewport{ m_viewport.x, m_viewport.y + m_viewport.height/2,
        m_viewport.width, m_viewport.height/2 };
};

void CPUStatsWidget::clear() const {
    glViewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
    scissorClear(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
}

void CPUStatsWidget::draw() const {
    if (!m_visible) return;

    clear();

    drawWidgetBackground();

    if (m_cpuMeasure->getCoreTempInfoSuccess()) {
        drawCoreGraphs();
        drawStats();
    } else {
        drawNoInfoState();
    }
}

void CPUStatsWidget::drawNoInfoState() const {
    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);
    drawTopSerifLine(-1.0f, 1.0f);
    drawBottomSerifLine(-1.0f, 1.0f);

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    m_fontManager->renderLine(RG_FONT_TIME, "No Data", 0, 0, 0, 0,
            RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_CENTERED_VERTICAL, 0, 0);
}

void CPUStatsWidget::drawStats() const {
    glViewport(m_statsViewport.x, m_statsViewport.y,
               m_statsViewport.width, m_statsViewport.height);

    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);
    drawTopSerifLine(-1.0f, 1.0f);
    drawBottomSerifLine(-1.0f, 1.0f);

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    const auto fontHeight{ m_fontManager->getFontCharHeight(RG_FONT_STANDARD) };
    constexpr auto bottomTextMargin{ 10U };

    char voltBuff[7];
    char clockBuff[12];
    snprintf(voltBuff, sizeof(voltBuff), "%.3fv", m_cpuMeasure->getVoltage());
    snprintf(clockBuff, sizeof(clockBuff), "%7.3fMHz",
             m_cpuMeasure->getClockSpeed());

    m_fontManager->renderLine(RG_FONT_STANDARD, voltBuff, 0, 0, 0, 0,
                              RG_ALIGN_LEFT | RG_ALIGN_BOTTOM,
                              bottomTextMargin, bottomTextMargin);
    m_fontManager->renderLine(RG_FONT_STANDARD, clockBuff, 0, 0, 0, 0,
                              RG_ALIGN_RIGHT | RG_ALIGN_BOTTOM,
                              bottomTextMargin, bottomTextMargin);
}

void CPUStatsWidget::drawCoreGraphs() const {
    glViewport(m_coreGraphViewport.x, m_coreGraphViewport.y,
               m_coreGraphViewport.width, m_coreGraphViewport.height);

    // Draw x rows of core graphs, with 2 graphs per row until all graphs
    // are drawn
    const auto numCores{ m_cpuMeasure->getPerCoreUsageData().size() };
    const auto numRows{ numCores / 2 }; // Fair to assume we have an even number of cores

    glLineWidth(0.5f);
    glColor4f(GRAPHLINE_A, GRAPHLINE_G, GRAPHLINE_B, GRAPHLINE_A);
    for (auto i = size_t{ 0U }; i < numCores; ++i) {
        // Set the viewport for the current graph. The y position
        // of each graph changes as we draw more
        const auto yOffset{ (numCores - 1) * m_coreGraphViewport.height/numCores - 
            i*m_coreGraphViewport.height/numCores };
        glViewport(m_coreGraphViewport.x, 
                   m_coreGraphViewport.y + yOffset,
                   3*m_coreGraphViewport.width/4,
                   m_coreGraphViewport.height/numCores);

        drawLineGraph(m_cpuMeasure->getPerCoreUsageData()[i]);

        // Draw the border for this core graph
        drawBorder();
        drawGraphGrid();

        // Draw a label for the core graph
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
        char str[7];
        snprintf(str, sizeof(str), "Core %d", i);
        m_fontManager->renderLine(RG_FONT_SMALL, str, 0, 0, 0, 0,
                                  RG_ALIGN_TOP | RG_ALIGN_LEFT, 10, 10);

        // Draw the temperature next to the graph
        glViewport(m_coreGraphViewport.x + 3*m_coreGraphViewport.width/4,
                   m_coreGraphViewport.y + yOffset,
                   m_coreGraphViewport.width/4,
                   m_coreGraphViewport.height/numCores);
        char tempBuff[6];
        snprintf(tempBuff, sizeof(tempBuff), "%.0fC", m_cpuMeasure->getTemp(i));
        m_fontManager->renderLine(RG_FONT_MUSIC_LARGE, tempBuff, 0, 0, 0, 0,
                                  RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_CENTERED_VERTICAL,
                                  0, 0);
    }
}

void CPUStatsWidget::setVisibility(bool b) {
    m_visible = b;
    if (m_visible) {
        draw();
    } else {
        clear();
    }
}

}
