#include "../headers/HDDWidget.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <Windows.h>

#include "../headers/colors.h"
#include "../headers/FontManager.h"
#include "../headers/DriveMeasure.h"

namespace rg {

HDDWidget::HDDWidget() {

}

HDDWidget::~HDDWidget() {

}

void HDDWidget::init(const FontManager* fontManager,
                     const DriveMeasure* driveMeasure,
                     Viewport viewport) {

    m_fontManager = fontManager;
    m_driveMeasure = driveMeasure;
    m_viewport = viewport;
}

void HDDWidget::draw() const {
    glViewport(m_viewport.x, m_viewport.y,
               m_viewport.width, m_viewport.height);

    glColor3f(DIVIDER_R, DIVIDER_G, DIVIDER_B);
    glLineWidth(0.5f);
    drawTopSerifLine(-1.0f, 1.0f);
    drawBottomSerifLine(-1.0f, 1.0f);

    // Draw each drive status section
    const auto& drives{ m_driveMeasure->getDrives() };
    for (auto i = size_t{ 0 }; i < drives.size(); ++i) {
        glViewport(m_viewport.x + i * (m_viewport.width / drives.size()), m_viewport.y,
                   m_viewport.width / drives.size(), m_viewport.height);

        // Draw the drive label on the bottom
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
        const char label[3]{ drives[i]->getDriveLetter(), ':', '\0' };
        m_fontManager->renderLine(RG_FONT_STANDARD, label, 0, 0, 0, 0,
                                  RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_BOTTOM,
                                  0, 10);

        // Draw the capacity up top
        m_fontManager->renderLine(RG_FONT_STANDARD,
                                  drives[i]->getCapacityStr().c_str(),
                                  0, 0, 0, 0,
                                  RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_TOP,
                                  0, 10);

        const auto percentage{ static_cast<float>(drives[i]->getUsedBytes()) /
                               drives[i]->getTotalBytes() };
        constexpr auto barWidth = float{ 0.3f };
        constexpr auto bottomY = float{ -0.5f };
        constexpr auto topY = float{ 0.5f };
        constexpr auto rangeY{ topY - bottomY };
        const float barStartX{ ((2.0f - barWidth) / 2.0f) - 1.0f };
        glBegin(GL_QUADS); {
            glColor3f(BARFILLED_R, BARFILLED_G, BARFILLED_B);
            glVertex2f(barStartX, bottomY);
            glVertex2f(barStartX, bottomY + percentage * rangeY);
            glVertex2f(barStartX + barWidth, bottomY + percentage * rangeY);
            glVertex2f(barStartX + barWidth, bottomY);

            glColor3f(BARFREE_R, BARFREE_G, BARFREE_B);
            glVertex2f(barStartX, bottomY + percentage * rangeY);
            glVertex2f(barStartX, topY);
            glVertex2f(barStartX + barWidth, topY);
            glVertex2f(barStartX + barWidth, bottomY + percentage * rangeY);
        } glEnd();
    }
}

}
