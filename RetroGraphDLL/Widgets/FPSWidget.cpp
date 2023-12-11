module Widgets.FPSWidget;

import Colors;

namespace rg {

void FPSWidget::draw() const {
    const auto fps{ m_fpsCounter->getFPS() };
    if (fps < 1000.0f) {
        char fpsStr[5];
        snprintf(fpsStr, sizeof(fpsStr), "%.1f", fps);
        m_fontManager->renderLine(RG_FONT_STANDARD_BOLD, fpsStr, 0, 0, 0, 0,
                                  RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_CENTERED_VERTICAL);
    } else {
        m_fontManager->renderLine(RG_FONT_STANDARD_BOLD, 
                                  std::to_string(static_cast<int>(fps)).c_str(),
                                  0, 0, 0, 0,
                                  RG_ALIGN_CENTERED_HORIZONTAL | RG_ALIGN_CENTERED_VERTICAL);
    }
}

} // namespace rg
