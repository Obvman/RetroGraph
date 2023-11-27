module Widgets.RAMGraphWidget;

import Colors;

namespace rg {

RAMGraphWidget::RAMGraphWidget(const FontManager* fontManager, std::shared_ptr<const RAMMeasure> ramMeasure)
    : Widget{ fontManager }
    , m_ramMeasure{ ramMeasure }
    , m_postUpdateHandle{ RegisterPostUpdateCallback() }
    , m_graph{ ramMeasure->getUsageData().size() } {

}

RAMGraphWidget::~RAMGraphWidget() {
    m_ramMeasure->postUpdate.remove(m_postUpdateHandle);
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

PostUpdateCallbackHandle RAMGraphWidget::RegisterPostUpdateCallback() {
    return m_ramMeasure->postUpdate.append(
        [this]() {
            m_graph.updatePoints(m_ramMeasure->getUsageData());
        });
}

} // namespace rg
