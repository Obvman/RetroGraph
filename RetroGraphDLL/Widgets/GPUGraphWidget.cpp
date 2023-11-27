module Widgets.GPUGraphWidget;

import Colors;

namespace rg {

GPUGraphWidget::GPUGraphWidget(const FontManager* fontManager, std::shared_ptr<const GPUMeasure> gpuMeasure)
    : Widget{ fontManager }
    , m_gpuMeasure{ gpuMeasure }
    , m_postUpdateHandle{ RegisterPostUpdateCallback() }
    , m_graph{ gpuMeasure->getUsageData().size() } {

}

GPUGraphWidget::~GPUGraphWidget() {
    m_gpuMeasure->postUpdate.remove(m_postUpdateHandle);
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

    m_fontManager->renderLine(RG_FONT_SMALL, "0%", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_BOTTOM | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "GPU Load", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_CENTERED_VERTICAL | RG_ALIGN_LEFT, 10);
    m_fontManager->renderLine(RG_FONT_SMALL, "100%", 0, 0, m_viewport.width/5, m_viewport.height,
                             RG_ALIGN_TOP | RG_ALIGN_LEFT, 10);
}

PostUpdateCallbackHandle GPUGraphWidget::RegisterPostUpdateCallback() {
    return m_gpuMeasure->postUpdate.append(
        [this]() {
            if (m_gpuMeasure->isEnabled()) {
                m_graph.updatePoints(m_gpuMeasure->getUsageData());
            }
        });
}

} // namespace rg
