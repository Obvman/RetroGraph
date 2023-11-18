export module Widgets.MainWidget;

import IRetroGraph; // Reverse Dependency

import Measures.AnimationState;

import Rendering.FontManager;
import Rendering.VBO;

import Widgets.Widget;

namespace rg {

export class MainWidget : public Widget {
public:
    MainWidget(const FontManager* fontManager, const IRetroGraph& rg, bool visible);
    ~MainWidget() noexcept = default;
    MainWidget(const MainWidget&) = delete;
    MainWidget& operator=(const MainWidget&) = delete;
    MainWidget(MainWidget&&) = delete;
    MainWidget& operator=(MainWidget&&) = delete;

    /* Checks if the widget should draw to maintain the target FPS */
    bool needsDraw(int ticks) const;
    void updateObservers(const IRetroGraph& rg) override;
    void draw() const override;
private:
    void drawParticles() const;
    void drawParticleLines() const;

    const AnimationState* m_animationState;

    VBOID m_vbo;
};

} // namespace rg
