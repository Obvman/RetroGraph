#include "../headers/HDDWidget.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <Windows.h>

#include "../headers/colors.h"
#include "../headers/FontManager.h"
#include "../headers/DriveMeasure.h"

namespace rg {

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
        glViewport(m_viewport.x + i * (m_viewport.width / drives.size()),
                   m_viewport.y, 
                   m_viewport.width / drives.size(),
                   m_viewport.height);

        // Draw the drive label on the bottom
        glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
        const char label[3]{ drives[i]->driveLetter, ':', '\0' };
        m_fontManager->renderLine(RG_FONT_STANDARD, label, 0, 0, 0, 0,
                RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_BOTTOM, 0, 10);

        // Draw the capacity up top
        m_fontManager->renderLine(RG_FONT_STANDARD,
                drives[i]->capacityStr.c_str(), 0, 0, 0, 0,
                RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_TOP, 0, 10);
        drawVerticalProgressBar(0.3f, -0.5f, 0.5f,
                static_cast<float>(drives[i]->totalBytes - drives[i]->totalFreeBytes),
                static_cast<float>(drives[i]->totalBytes));
    }
}

}
