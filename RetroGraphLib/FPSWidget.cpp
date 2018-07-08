#include "stdafx.h"

#include "FPSWidget.h"

// #include <gl/glew.h>
// #include <gl/GL.h>

#include "colors.h"
#include "FontManager.h"
#include "FPSLimiter.h"
#include "ListContainer.h"

namespace rg {

void FPSWidget::draw() const {
    if (!isVisible()) return;
    clear();

    drawWidgetBackground();

    ListContainer::inst().drawTopAndBottomSerifs();

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