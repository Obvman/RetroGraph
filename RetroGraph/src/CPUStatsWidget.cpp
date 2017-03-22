#include "../headers/CPUStatsWidget.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <Windows.h>

#include "../headers/utils.h"
#include "../headers/colors.h"
#include "../headers/FontManager.h"
#include "../headers/CPUMeasure.h"

namespace rg {

CPUStatsWidget::CPUStatsWidget() {

}

CPUStatsWidget::~CPUStatsWidget() {

}

void CPUStatsWidget::init(const FontManager* fontManager,
                         const CPUMeasure* cpuMeasure,
                         Viewport viewport) {

    m_fontManager = fontManager;
    m_cpuMeasure = cpuMeasure;
    m_viewport = viewport;
    m_coreGraphViewport = Viewport{ m_viewport.x, m_viewport.y, 
                                    m_viewport.width, m_viewport.height/2 - 5 };
    m_statsViewport = Viewport{ m_viewport.x, m_viewport.y + m_viewport.height/2+5,
                                m_viewport.width, m_viewport.height/2 };
    // TODO what is that 5 for?
}

void CPUStatsWidget::draw() const {
    glViewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
    drawCoreGraphs();
    drawStats();
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
    snprintf(clockBuff, sizeof(clockBuff), "%7.3fMHz", m_cpuMeasure->getClockSpeed());

    m_fontManager->renderLine(RG_FONT_STANDARD, voltBuff, 0, 0, 0, 0,
            RG_ALIGN_LEFT | RG_ALIGN_BOTTOM, bottomTextMargin, bottomTextMargin);
    m_fontManager->renderLine(RG_FONT_STANDARD, clockBuff, 0, 0, 0, 0,
            RG_ALIGN_RIGHT | RG_ALIGN_BOTTOM, bottomTextMargin, bottomTextMargin);

    // Draw the temperature bar for each CPU core
    const auto numCores{ m_cpuMeasure->getNumCores() };
    for (auto i = size_t{ 0U }; i < numCores; ++i) {

        glViewport(m_statsViewport.x + i * m_statsViewport.width/numCores,
                   // Leave space for text below the bars
                   m_statsViewport.y + fontHeight + bottomTextMargin,
                   m_statsViewport.width / numCores,
                   m_statsViewport.height - fontHeight - bottomTextMargin);

        constexpr auto maxTemp = float{ 100.0f };
        constexpr auto barWidth = float{ 0.3f };
        constexpr auto bottomY = float{ -0.7f };
        constexpr auto topY = float{ 0.7f };
        constexpr auto rangeY { topY - bottomY };
        constexpr auto barStartX{ ((2.0f - barWidth) / 2.0f) - 1.0f };
        const auto percentage = float{ m_cpuMeasure->getTemp(i) / m_cpuMeasure->getTjMax() };

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
        m_fontManager->renderLine(RG_FONT_STANDARD, coreBuff, 0, 0, 0, 0,
                                 RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_BOTTOM,
                                 10, 10);
        char tempBuff[6];
        snprintf(tempBuff, sizeof(tempBuff), "%.0fC", m_cpuMeasure->getTemp(i));
        m_fontManager->renderLine(RG_FONT_STANDARD, tempBuff, 0, 0, 0, 0,
                                 RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_TOP,
                                 10, 10);
    }
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
        glViewport(m_coreGraphViewport.x,
                   (m_coreGraphViewport.y + (numCores-1)*m_coreGraphViewport.height/numCores)
                   - i*m_coreGraphViewport.height/(numCores),
                   m_coreGraphViewport.width,
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
    }
}

}
