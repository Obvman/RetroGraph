module Widgets.CPUGraphWidget;

import Colors;

namespace rg {

CPUGraphWidget::CPUGraphWidget(const FontManager* fontManager, std::shared_ptr<const CPUMeasure> cpuMeasure)
    : Widget{ fontManager }
    , m_cpuMeasure{ cpuMeasure }
    , m_onCPUUsageHandle{ RegisterCPUUsageCallback() }
    , m_graphSampleSize{ UserSettings::inst().getVal<int>("Widgets-CPUGraph.NumUsageSamples") }
    , m_graph{ static_cast<size_t>(m_graphSampleSize) }
    , m_configRefreshedHandle{ RegisterConfigRefreshedCallback() } {

}

CPUGraphWidget::~CPUGraphWidget() {
    UserSettings::inst().configRefreshed.remove(m_configRefreshedHandle);
    m_cpuMeasure->onCPUUsage.remove(m_onCPUUsageHandle);
}

void CPUGraphWidget::draw() const {
    // Set the viewport for the graph to be left section
    glViewport(m_viewport.x, m_viewport.y, (m_viewport.width * 4)/5, m_viewport.height);
    m_graph.draw();

    // Text
    glViewport(m_viewport.x + (4 * m_viewport.width) / 5, m_viewport.y, m_viewport.width / 5, m_viewport.height);

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    m_fontManager->renderLine(RG_FONT_SMALL, "0%", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "CPU Load", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "100%", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
}

CPUUsageCallbackHandle CPUGraphWidget::RegisterCPUUsageCallback() {
    return m_cpuMeasure->onCPUUsage.append([this](float usage) { m_graph.addPoint(usage); });
}

ConfigRefreshedCallbackHandle CPUGraphWidget::RegisterConfigRefreshedCallback() {
    return UserSettings::inst().configRefreshed.append(
        [this]() {
            const int newGraphSampleSize{ UserSettings::inst().getVal<int>("Widgets-CPUGraph.NumUsageSamples") };
            if (m_graphSampleSize != newGraphSampleSize) {
                m_graphSampleSize = newGraphSampleSize;
                m_graph.resetPoints(m_graphSampleSize);
            }
        }
    );
}

} // namespace rg
