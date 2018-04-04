#include "../headers/FPSWidget.h"

#include <gl/glew.h>
#include <gl/GL.h>

#include "../headers/colors.h"
#include "../headers/FontManager.h"
#include "../headers/FPSLimiter.h"

namespace rg {

void FPSWidget::draw() const {
    if (!m_visible) return;
    clear();

    glColor4f(DIVIDER_R, DIVIDER_G, DIVIDER_B, DIVIDER_A);
    drawTopSerifLine(-1.0f, 1.0f);
    drawBottomSerifLine(-1.0f, 1.0f);

    const auto fps{ FPSLimiter::inst().getFPS() };
    if (fps < 1000.0f) {
        char fpsStr[5];
        snprintf(fpsStr, sizeof(fpsStr), "%.1f", fps);
        m_fontManager->renderLine(RG_FONT_STANDARD_BOLD, fpsStr, 0, 0, 0, 0,
                                  RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_CENTERED_VERTICAL);
    } else {
        m_fontManager->renderLine(RG_FONT_STANDARD_BOLD, 
                                  std::to_string(static_cast<uint32_t>(fps)).c_str(),
                                  0, 0, 0, 0,
                                  RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_CENTERED_VERTICAL);
    }



}

void FPSWidget::clear() const {
    glViewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
    scissorClear(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
}

void FPSWidget::setVisibility(bool b) {
    m_visible = b;
    if (m_visible) {
        draw();
    } else {
        clear();
    }
}

}