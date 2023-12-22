module RG.Widgets:HDDWidget;

import Colors;

import RG.Core;
import RG.Rendering;

import "GLHeaderUnit.h";

namespace rg {

HDDWidget::HDDWidget(const FontManager* fontManager,
                     std::shared_ptr<const DriveMeasure> driveMeasure)
    : Widget{ fontManager }
    , m_driveMeasure{ driveMeasure }
    , m_postUpdateHandle{ RegisterPostUpdateCallback() } {

}

HDDWidget::~HDDWidget() {
    m_driveMeasure->postUpdate.detach(m_postUpdateHandle);
}

void HDDWidget::draw() const {
    // Draw each drive status section
    const auto& drives{ m_driveMeasure->getDrives() };
    const auto numDrives{ static_cast<GLsizei>(drives.size()) };
    for (int i = 0; i < numDrives; ++i) {
        glViewport(m_viewport.x + i * (m_viewport.width / numDrives),
                   m_viewport.y,
                   m_viewport.width / numDrives,
                   m_viewport.height);

        // Draw the drive label on the bottom
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
        const char label[3]{ drives[i].driveLetter, ':', '\0' };
        m_fontManager->renderLine(RG_FONT_STANDARD, label, 0, 0, 0, 0,
                                  RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_BOTTOM, 0, 10);

        // Draw the capacity up top
        m_fontManager->renderLine(RG_FONT_STANDARD,
                                  getCapacityStr(drives[i].totalBytes).c_str(), 0, 0, 0, 0,
                                  RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_TOP, 0, 10);

        drawVerticalProgressBar(0.3f, -0.5f, 0.5f,
                                static_cast<float>(drives[i].totalBytes - drives[i].totalFreeBytes),
                                static_cast<float>(drives[i].totalBytes), true);
    }
}

std::string HDDWidget::getCapacityStr(uint64_t capacityBytes) const {
    const auto capacityGB{ bToGB(capacityBytes) };
    if (capacityGB < 1000) {
        return std::format("{}GB", capacityGB);
    } else {
        return std::format("{:.1f}TB", capacityGB / 1024.0f);
    }
}

PostUpdateEvent::Handle HDDWidget::RegisterPostUpdateCallback() {
    return m_driveMeasure->postUpdate.attach(
        [this]() {
            invalidate();
        });
}

} // namespace rg
