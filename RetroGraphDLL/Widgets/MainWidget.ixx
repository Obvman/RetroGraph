export module Widgets.MainWidget;

import Measures.AnimationState;
import Measures.ParticleLine;

import Rendering.FontManager;
import Rendering.GLShader;
import Rendering.VAO;
import Rendering.VBO;

import Widgets.Widget;

import std.memory;

import "GLHeaderUnit.h";

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
    void drawParticles(float aspectRatio) const;
    void drawParticleLines(float aspectRatio) const;

    void createParticleVAO();
    void createParticleLinesVAO(size_t numLines);

    void updateParticleLinesVAO() const;

    std::shared_ptr<const AnimationState> m_animationState;

    VAO m_particleLinesVAO;
    mutable VBO<ParticleLine> m_particleLinesVBO; //#TODO mutable
    GLShader m_particleLinesShader;

    VAO m_particleVAO;
    VBO<glm::vec2> m_particleVBO;
    GLShader m_particleShader;
};

} // namespace rg
