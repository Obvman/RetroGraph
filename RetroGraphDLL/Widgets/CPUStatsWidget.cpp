module Widgets.CPUStatsWidget;

import Colors;

import Rendering.DrawUtils;

import "GLHeaderUnit.h";
import "RGAssert.h";

namespace rg {

auto CPUStatsWidget::createCoreGraphs(const CPUMeasure& cpuMeasure) {
    decltype(m_coreGraphs) coreGraphs{};

    coreGraphs.resize(cpuMeasure.getPerCoreUsageData().size());

    return coreGraphs;
}

CPUStatsWidget::CPUStatsWidget(const FontManager* fontManager, std::shared_ptr<const CPUMeasure> cpuMeasure)
    : Widget{ fontManager }
    , m_cpuMeasure{ cpuMeasure }
    , m_coreGraphs{ createCoreGraphs(*cpuMeasure) }
    , m_postUpdateHandle{ RegisterPostUpdateCallback() } {

}

CPUStatsWidget::~CPUStatsWidget() {
    m_cpuMeasure->postUpdate.remove(m_postUpdateHandle);
}

void CPUStatsWidget::setViewport(const Viewport& vp) { 
    m_viewport = vp;
    m_coreGraphViewport = {vp.x, vp.y, (vp.width / 4) * 3, vp.height};
    m_statsViewport = {m_coreGraphViewport.width + vp.x, vp.y,
                       (vp.width / 4), vp.height};

};

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
    m_fontManager->renderLine(RG_FONT_TIME, "No Data", 0, 0, 0, 0, RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_CENTERED_VERTICAL, 0, 0);
}

void CPUStatsWidget::drawStats() const {
    setGLViewport(m_statsViewport);

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
    setGLViewport(m_coreGraphViewport);

    // Draw x rows of core graphs, with 2 graphs per row until all graphs
    // are drawn
    const auto numGraphs{ static_cast<int>(m_coreGraphs.size()) };

    for (int i = 0U; i < numGraphs; ++i) {
        // Set the viewport for the current graph. Draws top to bottom
        const auto yOffset{ (numGraphs - 1) * m_coreGraphViewport.height/numGraphs - i*m_coreGraphViewport.height/numGraphs };
        glViewport(m_coreGraphViewport.x, 
                   m_coreGraphViewport.y + yOffset,
                   3 * m_coreGraphViewport.width / 4,
                   m_coreGraphViewport.height / numGraphs);

        m_coreGraphs[i].draw();

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
                   m_coreGraphViewport.height/static_cast<GLsizei>(numGraphs));
        char tempBuff[6];
        snprintf(tempBuff, sizeof(tempBuff), "%.0fC", m_cpuMeasure->getTemp(i));
        m_fontManager->renderLine(RG_FONT_SMALL, tempBuff, 0, 0, 0, 0,
                                  RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_CENTERED_VERTICAL,
                                  0, 0);
    }
}

PostUpdateCallbackHandle CPUStatsWidget::RegisterPostUpdateCallback() {
    return m_cpuMeasure->postUpdate.append(
        [this]() {
            if (m_coreGraphs.size() != m_cpuMeasure->getPerCoreUsageData().size()) {
                m_coreGraphs = createCoreGraphs(*m_cpuMeasure);
            }

            const auto numCores{ static_cast<int>(m_cpuMeasure->getPerCoreUsageData().size()) };
            RGASSERT(numCores == m_coreGraphs.size(), "CPU core count mismatch");
            for (int i = 0U; i < numCores; ++i) {
                m_coreGraphs[i].updatePoints(m_cpuMeasure->getPerCoreUsageData()[i]);
            }
        });
}

} // namespace rg
