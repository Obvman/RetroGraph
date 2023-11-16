module;

export module Widgets.HDDWidget;

import Colors;
import IRetroGraph; // Reverse Dependency

import Measures.DriveMeasure;

import Rendering.FontManager;
import Rendering.GLListContainer;

import Widgets.Widget;

import "GLHeaders.h";

namespace rg {

export { class HDDWidget; }

class HDDWidget : public Widget {
public:
    HDDWidget(const FontManager* fontManager,
              const IRetroGraph& rg, bool visible) :
        Widget{ fontManager, visible },
        m_driveMeasure{ &rg.getDriveMeasure() } { }

    ~HDDWidget() noexcept = default;
    HDDWidget(const HDDWidget&) = delete;
    HDDWidget& operator=(const HDDWidget&) = delete;
    HDDWidget(HDDWidget&&) = delete;
    HDDWidget& operator=(HDDWidget&&) = delete;

    void updateObservers(const IRetroGraph& rg) override;
    void draw() const override;
private:
    const DriveMeasure* m_driveMeasure;
};

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
                static_cast<float>(drives[i].totalBytes -
                    drives[i].totalFreeBytes),
                static_cast<float>(drives[i].totalBytes), true);
    }
}

}
