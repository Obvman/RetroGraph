#include "stdafx.h"

#include "FPSWidget.h"

// #include <gl/glew.h>
// #include <gl/GL.h>

#include "colors.h"
#include "FontManager.h"
#include "FPSLimiter.h"

namespace rg {

void FPSWidget::draw() const {
    if (!m_visible) return;
    clear();

    drawWidgetBackground();

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
}