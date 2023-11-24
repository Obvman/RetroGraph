export module Widgets.MainWidget;

import Measures.AnimationState;
import Measures.ParticleLine;

import Rendering.FontManager;
import Rendering.GLShader;
import Rendering.VAO;
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
    void reloadShaders() override;

private:
    void drawParticles() const;
    void drawParticleLines(float aspectRatio) const;

    void createAnimationVBO(size_t numLines);
    void updateAnimationVBO() const;
    void drawAnimationVBO(int size, float aspectRatio) const;

    std::shared_ptr<const AnimationState> m_animationState;

    VAO m_animVAO;
    mutable VBO<ParticleLine> m_animLines; //#TODO mutable
    GLShader m_animShader;
};

} // namespace rg
