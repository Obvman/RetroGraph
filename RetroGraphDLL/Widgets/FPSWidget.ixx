export module Widgets.FPSWidget;

import FPSCounter;

import Rendering.FontManager;

import Widgets.Widget;

namespace rg {

export class FPSWidget : public Widget {
public:
    FPSWidget(const FontManager* fontManager, const FPSCounter& fpsLimiter)
        : Widget{ fontManager }
        , m_fpsCounter{ &fpsLimiter } {
    }
    ~FPSWidget() noexcept = default;

    void draw() const override;

private:
    const FPSCounter* m_fpsCounter;
};

}
