#include "stdafx.h"

#include "CPUStatsWidget.h"

// #include <GL/glew.h>
// #include <GL/gl.h>
#include <Windows.h>

#include "utils.h"
#include "colors.h"
#include "FontManager.h"
#include "CPUMeasure.h"
#include "RetroGraph.h"
#include "ListContainer.h"
#include "VBOController.h"

namespace rg {

void CPUStatsWidget::setViewport(const Viewport& vp) { 
    m_viewport = vp;

    // Hack to fix rounding issue
    const auto coreY{ static_cast<GLint>(std::floor(m_viewport.height / 2.0)) };
    m_coreGraphViewport = Viewport{ 
        m_viewport.x,
        m_viewport.y,
        m_viewport.width,
        coreY
    };
    m_statsViewport = Viewport{ 
        m_viewport.x,
        m_viewport.y + coreY,
        m_viewport.width,
        static_cast<GLint>(std::ceil(m_viewport.height/2.0))
    };
};


void CPUStatsWidget::updateObservers(const RetroGraph & rg) {
    m_cpuMeasure = &rg.getCPUMeasure();
}

void CPUStatsWidget::draw() const {
    if (!isVisible()) return;

    Widget::clear();

    drawWidgetBackground();

    if (m_cpuMeasure->getCoreTempInfoSuccess()) {
        drawCoreGraphs();
        drawStats();
    } else {
        drawNoInfoState();
    }
}

void CPUStatsWidget::drawNoInfoState() const {
    ListContainer::inst().drawTopAndBottomSerifs();

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    m_fontManager->renderLine(RG_FONT_TIME, "No Data", 0, 0, 0, 0,
            RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_CENTERED_VERTICAL, 0, 0);
}

void CPUStatsWidget::drawStats() const {
    glViewport(m_statsViewport.x, m_statsViewport.y,
               m_statsViewport.width, m_statsViewport.height);

    ListContainer::inst().drawTopAndBottomSerifs();

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

    ListContainer::inst().drawTopAndBottomSerifs();

    // Draw x rows of core graphs, with 2 graphs per row until all graphs
    // are drawn
    const auto numCores{ m_cpuMeasure->getPerCoreUsageData().size() };
    const auto numRows{ numCores / 2 }; // Fair to assume we have an even number of cores

    glLineWidth(0.5f);
    glColor4f(GRAPHLINE_A, GRAPHLINE_G, GRAPHLINE_B, GRAPHLINE_A);
    for (unsigned int i = 0U; i < numCores; ++i) {
        // Set the viewport for the current graph. The y position
        // of each graph changes as we draw more
        const auto yOffset{ static_cast<GLint>((numCores - 1) * m_coreGraphViewport.height/numCores - 
            i*m_coreGraphViewport.height/numCores) };
        glViewport(m_coreGraphViewport.x, 
                   m_coreGraphViewport.y + yOffset,
                   3*m_coreGraphViewport.width/4,
                   m_coreGraphViewport.height / static_cast<int>(numCores));

        drawLineGraph(m_cpuMeasure->getPerCoreUsageData()[i]);

        // Draw the border for this core graph
        ListContainer::inst().drawBorder();
        VBOController::inst().drawGraphGrid();

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
                   m_coreGraphViewport.height/static_cast<GLsizei>(numCores));
        char tempBuff[6];
        snprintf(tempBuff, sizeof(tempBuff), "%.0fC", m_cpuMeasure->getTemp(i));
        m_fontManager->renderLine(RG_FONT_MUSIC_LARGE, tempBuff, 0, 0, 0, 0,
                                  RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_CENTERED_VERTICAL,
                                  0, 0);
    }
}

}
