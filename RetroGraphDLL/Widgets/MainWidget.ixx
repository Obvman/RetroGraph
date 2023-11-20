export module Widgets.MainWidget;

import Measures.AnimationState;

import Rendering.FontManager;
import Rendering.VBO;

import Widgets.Widget;

import std.memory;

namespace rg {

export class MainWidget : public Widget {
public:
    MainWidget(const FontManager* fontManager, std::shared_ptr<const AnimationState> animationState);
    ~MainWidget() noexcept = default;

    /* Checks if the widget should draw to maintain the target FPS */
    bool needsDraw(int ticks) const;
    void draw() const override;
private:
    void drawParticles() const;
    void drawParticleLines() const;

    std::shared_ptr<const AnimationState> m_animationState;

    VBOID m_vbo;
};

} // namespace rg
