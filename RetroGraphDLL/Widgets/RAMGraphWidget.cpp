module RG.Widgets:RAMGraphWidget;

import Colors;
import UserSettings;

namespace rg {

RAMGraphWidget::RAMGraphWidget(const FontManager* fontManager, std::shared_ptr<const RAMMeasure> ramMeasure)
    : Widget{ fontManager }
    , m_ramMeasure{ ramMeasure }
    , m_onRAMUsageHandle{ RegisterRAMUsageCallback() }
    , m_graphSampleSize{ UserSettings::inst().getVal<int>("Widgets-RAMGraph.NumUsageSamples") }
    , m_graph{ static_cast<size_t>(m_graphSampleSize) }
    , m_configRefreshedHandle{ RegisterConfigRefreshedCallback() } {

}

RAMGraphWidget::~RAMGraphWidget() {
    UserSettings::inst().configRefreshed.detach(m_configRefreshedHandle);
    m_ramMeasure->onRAMUsage.detach(m_onRAMUsageHandle);
}

void RAMGraphWidget::draw() const {
    // Set the viewport for the graph itself to be left section
    glViewport(m_viewport.x, m_viewport.y, (m_viewport.width*4)/5, m_viewport.height);
    m_graph.draw();

    // Set viewport for text drawing
    glViewport(m_viewport.x + (4 * m_viewport.width) / 5, m_viewport.y,
               m_viewport.width / 5, m_viewport.height);
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    m_fontManager->renderLine(RG_FONT_SMALL, "0%", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "RAM Load", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "100%", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);

}

RAMUsageEvent::Handle RAMGraphWidget::RegisterRAMUsageCallback() {
    return m_ramMeasure->onRAMUsage.attach(
        [this](float usage) {
            m_graph.addPoint(usage);
            invalidate();
        });
}

ConfigRefreshedEvent::Handle RAMGraphWidget::RegisterConfigRefreshedCallback() {
    return UserSettings::inst().configRefreshed.attach(
        [this]() {
            const int newGraphSampleSize{ UserSettings::inst().getVal<int>("Widgets-RAMGraph.NumUsageSamples") };
            if (m_graphSampleSize != newGraphSampleSize) {
                m_graphSampleSize = newGraphSampleSize;
                m_graph.resetPoints(m_graphSampleSize);
                invalidate();
            }
        }
    );
}

} // namespace rg
