module RG.Widgets:CPUStatsWidget;

import Colors;

import RG.Rendering;

import "GLHeaderUnit.h";
import "RGAssert.h";

namespace rg {

auto CPUStatsWidget::createCoreGraphs(const CPUMeasure& cpuMeasure) {
    decltype(m_coreGraphs) coreGraphs{};

    for (int i{ 0 }; i < cpuMeasure.getNumCores(); ++i)
        coreGraphs.emplace_back(static_cast<size_t>(m_coreGraphSampleSize));

    return coreGraphs;
}

CPUStatsWidget::CPUStatsWidget(const FontManager* fontManager, std::shared_ptr<const CPUMeasure> cpuMeasure)
    : Widget{ fontManager }
    , m_cpuMeasure{ cpuMeasure }
    , m_coreGraphSampleSize{ UserSettings::inst().getVal<int>("Widgets-CPUStats.NumUsageSamples") }
    , m_coreGraphs{ createCoreGraphs(*cpuMeasure) }
    , m_onCPUCoreUsageHandle{ RegisterOnCPUCoreUsageCallback() }
    , m_configRefreshedHandle{ RegisterConfigRefreshedCallback() } {}

CPUStatsWidget::~CPUStatsWidget() {
    UserSettings::inst().configRefreshed.detach(m_configRefreshedHandle);
    m_cpuMeasure->onCPUCoreUsage.detach(m_onCPUCoreUsageHandle);
}

void CPUStatsWidget::setViewport(const Viewport& vp) {
    m_viewport = vp;
    m_coreGraphViewport = { vp.x, vp.y, (vp.width / 4) * 3, vp.height };
    m_statsViewport = { m_coreGraphViewport.width + vp.x, vp.y, (vp.width / 4), vp.height };
};

void CPUStatsWidget::draw() const {
    drawCoreGraphs();
    drawStats();
}

void CPUStatsWidget::drawStats() const {
    setGLViewport(m_statsViewport);

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    constexpr auto bottomTextMargin{ 10U };

    char voltBuff[7];
    char clockBuff[12];
    snprintf(voltBuff, sizeof(voltBuff), "%.3fv", m_cpuMeasure->getVoltage());
    snprintf(clockBuff, sizeof(clockBuff), "%.0fMHz", m_cpuMeasure->getClockSpeed());

    m_fontManager->renderLine(RG_FONT_STANDARD, voltBuff, 0, 0, 0, 0, RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_TOP,
                              bottomTextMargin, bottomTextMargin);
    m_fontManager->renderLine(RG_FONT_STANDARD, clockBuff, 0, 0, 0, 0, RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_BOTTOM,
                              bottomTextMargin, bottomTextMargin);
}

void CPUStatsWidget::drawCoreGraphs() const {
    setGLViewport(m_coreGraphViewport);

    // Draw x rows of core graphs, with 2 graphs per row until all graphs
    // are drawn
    const auto numGraphs{ static_cast<int>(m_coreGraphs.size()) };

    for (int i = 0U; i < numGraphs; ++i) {
        // Set the viewport for the current graph. Draws top to bottom
        const auto yOffset{ (numGraphs - 1) * m_coreGraphViewport.height / numGraphs -
                            i * m_coreGraphViewport.height / numGraphs };
        glViewport(m_coreGraphViewport.x, m_coreGraphViewport.y + yOffset, 3 * m_coreGraphViewport.width / 4,
                   m_coreGraphViewport.height / numGraphs);

        m_coreGraphs[i].draw();

        // Draw a label for the core graph
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
        char str[7];
        snprintf(str, sizeof(str), "Core %d", i);
        m_fontManager->renderLine(RG_FONT_SMALL, str, 0, 0, 0, 0, RG_ALIGN_TOP | RG_ALIGN_LEFT, 10, 10);

        // Draw the temperature next to the graph
        glViewport(m_coreGraphViewport.x + 3 * m_coreGraphViewport.width / 4, m_coreGraphViewport.y + yOffset,
                   m_coreGraphViewport.width / 4, m_coreGraphViewport.height / static_cast<GLsizei>(numGraphs));
        char tempBuff[6];
        snprintf(tempBuff, sizeof(tempBuff), "%.0fC", m_cpuMeasure->getTemp(i));
        m_fontManager->renderLine(RG_FONT_SMALL, tempBuff, 0, 0, 0, 0,
                                  RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_CENTERED_VERTICAL, 0, 0);
    }
}

CPUCoreUsageEvent::Handle CPUStatsWidget::RegisterOnCPUCoreUsageCallback() {
    return m_cpuMeasure->onCPUCoreUsage.attach([this](int coreIdx, float coreUsage) {
        RGASSERT(coreIdx < m_coreGraphs.size(), "CPU core index out of range");

        if (m_coreGraphs.size() != m_cpuMeasure->getNumCores()) {
            RGERROR("How did the CPU core count change?");
            m_coreGraphs = createCoreGraphs(*m_cpuMeasure);
        }

        m_coreGraphs[coreIdx].addPoint(coreUsage);
        invalidate();
    });
}

ConfigRefreshedEvent::Handle CPUStatsWidget::RegisterConfigRefreshedCallback() {
    return UserSettings::inst().configRefreshed.attach([this]() {
        const int newGraphSampleSize{ UserSettings::inst().getVal<int>("Widgets-CPUStats.NumUsageSamples") };
        if (m_coreGraphSampleSize != newGraphSampleSize) {
            m_coreGraphSampleSize = newGraphSampleSize;
            for (auto& coreGraph : m_coreGraphs) {
                coreGraph.resetPoints(m_coreGraphSampleSize);
            }
            invalidate();
        }
    });
}

} // namespace rg
