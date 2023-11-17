export module Widgets.FPSWidget;

import Colors;
import FPSLimiter;
import IRetroGraph; // Reverse Dependency

import Rendering.DrawUtils;
import Rendering.FontManager;
import Rendering.GLListContainer;

import Widgets.Widget;

namespace rg {

export { class FPSWidget; }

class FPSWidget : public Widget {
public:
    FPSWidget(const FontManager* fontManager, bool visible) :
        Widget{ fontManager, visible } { }
    ~FPSWidget() noexcept = default;
    FPSWidget(const FPSWidget&) = delete;
    FPSWidget& operator=(const FPSWidget&) = delete;
    FPSWidget(FPSWidget&&) = delete;
    FPSWidget& operator=(FPSWidget&&) = delete;

    void updateObservers(const IRetroGraph&) override { /* Empty */ }
    void draw() const override;

};

void FPSWidget::draw() const {
    const auto fps{ FPSLimiter::inst().getFPS() };
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

}
