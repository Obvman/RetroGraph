export module RG.Widgets:FPSWidget;

import :Widget;

import FPSCounter;

import RG.Rendering;

namespace rg {

export class FPSWidget : public Widget {
public:
    FPSWidget(const FontManager* fontManager, const FPSCounter& fpsLimiter)
        : Widget{ fontManager }
        , m_fpsCounter{ &fpsLimiter } {}
    ~FPSWidget() noexcept = default;

    void draw() const override;

    bool needsRedraw() const { return true; } // draw every frame

private:
    const FPSCounter* m_fpsCounter;
};

} // namespace rg
