module Widgets.HDDWidget;

import Colors;

import Rendering.DrawUtils;

import "GLHeaderUnit.h";

namespace rg {

void HDDWidget::updateObservers(const IRetroGraph & rg) {
    m_driveMeasure = &rg.getDriveMeasure();
}

void HDDWidget::draw() const {
    // Draw each drive status section
    const auto& drives{ m_driveMeasure->getDrives() };
    const auto driveSize{ static_cast<GLsizei>(drives.size()) };
    for (int i = 0; i < driveSize; ++i) {
        glViewport(m_viewport.x + i * (m_viewport.width / driveSize),
                   m_viewport.y,
                   m_viewport.width / driveSize,
                   m_viewport.height);

        // Draw the drive label on the bottom
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
        const char label[3]{ drives[i].driveLetter, ':', '\0' };
        m_fontManager->renderLine(RG_FONT_STANDARD, label, 0, 0, 0, 0,
                                  RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_BOTTOM, 0, 10);

        // Draw the capacity up top
        m_fontManager->renderLine(RG_FONT_STANDARD,
                                  drives[i].capacityStr.c_str(), 0, 0, 0, 0,
                                  RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_TOP, 0, 10);

        drawVerticalProgressBar(0.3f, -0.5f, 0.5f,
                                static_cast<float>(drives[i].totalBytes - drives[i].totalFreeBytes),
                                static_cast<float>(drives[i].totalBytes), true);
    }
}

} // namespace rg
