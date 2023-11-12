module;

export module CPUStatsWidget;

import Colors;
import CPUMeasure;
import FontManager;
import GLListContainer;
import IRetroGraph; // Reverse Dependency
import Utils;
import VBOController;
import Widget;

import std.core;

import "GLHeaders.h";

namespace rg {

export { class CPUStatsWidget; }

/* Responsible for drawing widget containing CPU voltage, frequency and temperature
 * obtained from CoreTemp
 */
class CPUStatsWidget : public Widget {
public:
    CPUStatsWidget(const FontManager* fontManager, const IRetroGraph& rg,
                   bool visible) :
        Widget{ fontManager, visible },
        m_cpuMeasure{ &rg.getCPUMeasure() } {}

    ~CPUStatsWidget() noexcept = default;
    CPUStatsWidget(const CPUStatsWidget&) = delete;
    CPUStatsWidget& operator=(const CPUStatsWidget&) = delete;
    CPUStatsWidget(CPUStatsWidget&&) = delete;
    CPUStatsWidget& operator=(CPUStatsWidget&&) = delete;

    void updateObservers(const IRetroGraph& rg) override;
    void draw() const override;
    void setViewport(const Viewport& vp) override;

private:
    void drawCoreGraphs() const;
    void drawStats() const;
    void drawNoInfoState() const;

    Viewport m_coreGraphViewport{ };
    Viewport m_statsViewport{ };

    const CPUMeasure* m_cpuMeasure{ nullptr };
};

void CPUStatsWidget::setViewport(const Viewport& vp) { 
    m_viewport = vp;
    m_coreGraphViewport = {vp.x, vp.y, (vp.width / 4) * 3, vp.height};
    m_statsViewport = {m_coreGraphViewport.width + vp.x, vp.y,
                       (vp.width / 4), vp.height};

};


void CPUStatsWidget::updateObservers(const IRetroGraph & rg) {
    m_cpuMeasure = &rg.getCPUMeasure();
}

void CPUStatsWidget::draw() const {
    if (m_cpuMeasure->getCoreTempInfoSuccess()) {
        drawCoreGraphs();
        drawStats();
    } else {
        drawNoInfoState();
    }
}

void CPUStatsWidget::drawNoInfoState() const {
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    m_fontManager->renderLine(RG_FONT_TIME, "No Data", 0, 0, 0, 0,
            RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_CENTERED_VERTICAL, 0, 0);
}

void CPUStatsWidget::drawStats() const {
    viewport(m_statsViewport);

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    constexpr auto bottomTextMargin{ 10U };

    char voltBuff[7];
    char clockBuff[12];
    snprintf(voltBuff, sizeof(voltBuff), "%.3fv", m_cpuMeasure->getVoltage());
    snprintf(clockBuff, sizeof(clockBuff), "%.0fMHz", m_cpuMeasure->getClockSpeed());

    m_fontManager->renderLine(RG_FONT_STANDARD, voltBuff, 0, 0, 0, 0,
                              RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_TOP,
                              bottomTextMargin, bottomTextMargin);
    m_fontManager->renderLine(RG_FONT_STANDARD, clockBuff, 0, 0, 0, 0,
                              RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_BOTTOM,
                              bottomTextMargin, bottomTextMargin);
}

void CPUStatsWidget::drawCoreGraphs() const {
    viewport(m_coreGraphViewport);

    // Draw x rows of core graphs, with 2 graphs per row until all graphs
    // are drawn
    const auto numCores{ std::max(static_cast<int>(m_cpuMeasure->getPerCoreUsageData().size()), 4) };

    glLineWidth(0.5f);
    glColor4f(GRAPHLINE_A, GRAPHLINE_G, GRAPHLINE_B, GRAPHLINE_A);
    for (int i = 0U; i < numCores; ++i) {
        // Set the viewport for the current graph. Draws top to bottom
        const auto yOffset{ (numCores - 1) * m_coreGraphViewport.height/numCores - 
            i*m_coreGraphViewport.height/numCores };
        glViewport(m_coreGraphViewport.x, 
                   m_coreGraphViewport.y + yOffset,
                   3*m_coreGraphViewport.width/4,
                   m_coreGraphViewport.height / numCores);

        drawLineGraph(m_cpuMeasure->getPerCoreUsageData()[i]);

        // Draw the border for this core graph
        GLListContainer::inst().drawBorder();
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
