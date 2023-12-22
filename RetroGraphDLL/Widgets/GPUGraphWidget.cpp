module RG.Widgets:GPUGraphWidget;

import Colors;

namespace rg {

GPUGraphWidget::GPUGraphWidget(const FontManager* fontManager, std::shared_ptr<const GPUMeasure> gpuMeasure)
    : Widget{ fontManager }
    , m_gpuMeasure{ gpuMeasure }
    , m_onGPUUsageHandle{ RegisterGPUUsageCallback() }
    , m_graphSampleSize{ UserSettings::inst().getVal<int>("Widgets-GPUGraph.NumUsageSamples") }
    , m_graph{ static_cast<size_t>(m_graphSampleSize) }
    , m_configRefreshedHandle{ RegisterConfigRefreshedCallback() } {}

GPUGraphWidget::~GPUGraphWidget() {
    m_gpuMeasure->onGPUUsage.detach(m_onGPUUsageHandle);
    UserSettings::inst().configRefreshed.detach(m_configRefreshedHandle);
}

void GPUGraphWidget::draw() const {
    if (m_gpuMeasure->isEnabled()) {
        // Set the viewport for the graph to be left section
        glViewport(m_viewport.x, m_viewport.y, (m_viewport.width * 4) / 5, m_viewport.height);
        m_graph.draw();
    }

    // Set viewport for text drawing
    glViewport(m_viewport.x + (4 * m_viewport.width) / 5, m_viewport.y, m_viewport.width / 5, m_viewport.height);
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    m_fontManager->renderLine(RG_FONT_SMALL, "0%", 0, 0, m_viewport.width / 5, m_viewport.height,
                              RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "GPU Load", 0, 0, m_viewport.width / 5, m_viewport.height,
                              RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "100%", 0, 0, m_viewport.width / 5, m_viewport.height,
                              RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
}

GPUUsageEvent::Handle GPUGraphWidget::RegisterGPUUsageCallback() {
    return m_gpuMeasure->onGPUUsage.attach([this](float usage) {
        if (m_gpuMeasure->isEnabled()) {
            m_graph.addPoint(usage);
            invalidate();
        }
    });
}

ConfigRefreshedEvent::Handle GPUGraphWidget::RegisterConfigRefreshedCallback() {
    return UserSettings::inst().configRefreshed.attach([this]() {
        if (m_gpuMeasure->isEnabled()) {
            const int newGraphSampleSize{ UserSettings::inst().getVal<int>("Widgets-GPUGraph.NumUsageSamples") };
            if (m_graphSampleSize != newGraphSampleSize) {
                m_graphSampleSize = newGraphSampleSize;
                m_graph.resetPoints(m_graphSampleSize);
                invalidate();
            }
        }
    });
}

} // namespace rg
