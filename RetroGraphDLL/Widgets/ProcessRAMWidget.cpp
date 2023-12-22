module RG.Widgets:ProcessRAMWidget;

import Colors;

namespace rg {

ProcessRAMWidget::ProcessRAMWidget(const FontManager* fontManager, std::shared_ptr<const ProcessMeasure> processMeasure)
    : Widget{ fontManager }
    , m_procMeasure{ processMeasure }
    , m_postUpdateHandle{ RegisterPostUpdateCallback() } {}

ProcessRAMWidget::~ProcessRAMWidget() {
    m_procMeasure->postUpdate.detach(m_postUpdateHandle);
}

void ProcessRAMWidget::draw() const {
    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);

    auto procNames = std::vector<std::string>{};
    auto procRamUsages = std::vector<std::string>{};

    procNames.reserve(m_procMeasure->getProcRAMData().size());
    procRamUsages.reserve(m_procMeasure->getProcRAMData().size());
    for (const auto& pair : m_procMeasure->getProcRAMData()) {
        procNames.emplace_back(pair.first);

        // Convert RAM value to string format, assuming top RAM usages are only
        // ever in megabytes or gigabytes
        char buff[6];
        if (pair.second >= 1000) {
            snprintf(buff, sizeof(buff), "%.1fGB", pair.second / 1024.0f);
        } else {
            snprintf(buff, sizeof(buff), "%zdMB", pair.second);
        }
        procRamUsages.emplace_back(buff);
    }

    m_fontManager->renderLines(RG_FONT_STANDARD, procNames, 0, 0, 0, 0, RG_ALIGN_LEFT | RG_ALIGN_CENTERED_VERTICAL, 15,
                               5);
    m_fontManager->renderLines(RG_FONT_STANDARD, procRamUsages, 0, 0, 0, 0, RG_ALIGN_RIGHT | RG_ALIGN_CENTERED_VERTICAL,
                               15, 5);
}

PostUpdateEvent::Handle ProcessRAMWidget::RegisterPostUpdateCallback() {
    return m_procMeasure->postUpdate.attach([this]() { invalidate(); });
}

} // namespace rg
