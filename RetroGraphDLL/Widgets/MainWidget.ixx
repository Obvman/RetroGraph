export module Widgets.MainWidget;

import Measures.AnimationState;

import Rendering.FontManager;
import Rendering.VBO;

import Widgets.Widget;

import std.memory;

namespace rg {

export class MainWidget : public Widget {
public:
    MainWidget(const FontManager* fontManager, std::shared_ptr<AnimationState const> animationState, bool visible);
    ~MainWidget() noexcept = default;
    MainWidget(const MainWidget&) = delete;
    MainWidget& operator=(const MainWidget&) = delete;
    MainWidget(MainWidget&&) = delete;
    MainWidget& operator=(MainWidget&&) = delete;

    /* Checks if the widget should draw to maintain the target FPS */
    bool needsDraw(int ticks) const;
    void draw() const override;
private:
    void drawParticles() const;
    void drawParticleLines() const;

    std::shared_ptr<AnimationState const> m_animationState;

    VBOID m_vbo;
};

} // namespace rg
