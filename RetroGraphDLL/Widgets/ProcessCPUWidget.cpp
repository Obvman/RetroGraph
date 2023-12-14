module Widgets.ProcessCPUWidget;

import Colors;

namespace rg {

ProcessCPUWidget::ProcessCPUWidget(const FontManager* fontManager, std::shared_ptr<const ProcessMeasure> processMeasure)
    : Widget{ fontManager }
    , m_procMeasure{ processMeasure }
    , m_postUpdateHandle{ RegisterPostUpdateCallback() } {

}

ProcessCPUWidget::~ProcessCPUWidget() {
    m_procMeasure->postUpdate.detach(m_postUpdateHandle);
}

void ProcessCPUWidget::draw() const {
    // Draw the list itself
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    auto procNames = std::vector<std::string>{  };
    auto procPercentages = std::vector<std::string>{  };
    procNames.reserve(m_procMeasure->getProcCPUData().size());
    procPercentages.reserve(m_procMeasure->getProcCPUData().size());
    for (const auto& pair : m_procMeasure->getProcCPUData()) {
        procNames.emplace_back(pair.first);

        // Convert percentage to string format
        char buff[6];
        snprintf(buff, sizeof(buff), "%4.1f%%", pair.second);
        procPercentages.emplace_back(buff);
    }

    m_fontManager->renderLines(RG_FONT_STANDARD, procNames, 0, 0, 0, 0,
                               RG_ALIGN_LEFT | RG_ALIGN_CENTERED_VERTICAL,
                               15, 5);
    m_fontManager->renderLines(RG_FONT_STANDARD, procPercentages, 0, 0, 0, 0,
                               RG_ALIGN_RIGHT | RG_ALIGN_CENTERED_VERTICAL,
                               15, 5);
}

PostUpdateEvent::Handle ProcessCPUWidget::RegisterPostUpdateCallback() {
    return m_procMeasure->postUpdate.attach(
        [this]() {
            invalidate();
        });
}

} // namespace rg
