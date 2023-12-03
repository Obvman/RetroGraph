module Widgets.CPUGraphWidget;

import Colors;

namespace rg {

CPUGraphWidget::CPUGraphWidget(const FontManager* fontManager, std::shared_ptr<const CPUMeasure> cpuMeasure)
    : Widget{ fontManager }
    , m_cpuMeasure{ cpuMeasure }
    , m_postUpdateHandle{ RegisterPostUpdateCallback() }
    , m_graph{} {

}

CPUGraphWidget::~CPUGraphWidget() {
    m_cpuMeasure->postUpdate.remove(m_postUpdateHandle);
}

void CPUGraphWidget::draw() const {
    // Set the viewport for the graph to be left section
    glViewport(m_viewport.x, m_viewport.y, (m_viewport.width * 4)/5, m_viewport.height);
    m_graph.draw();
    m_graph2.draw();

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

PostUpdateCallbackHandle CPUGraphWidget::RegisterPostUpdateCallback() {
    return m_cpuMeasure->postUpdate.append(
        [this]() {
            m_graph.updatePoints(m_cpuMeasure->getUsageData());
            m_graph2.updatePoints(m_cpuMeasure->getUsageData());
        });
}

} // namespace rg
