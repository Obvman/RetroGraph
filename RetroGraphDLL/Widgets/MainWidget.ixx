export module Widgets.MainWidget;

import Measures.AnimationState;
import Measures.ParticleLine;
import Measures.Particle;

import Rendering.FontManager;
import Rendering.Shader;
import Rendering.VAO;
import Rendering.VBO;

import Widgets.Widget;

import std.memory;

import "GLHeaderUnit.h";

namespace rg {

#pragma pack(push)
#pragma pack(1)
struct ParticleRenderData {
    ParticleRenderData() = default;
    ParticleRenderData(const Particle& particle)
        : position{ particle.x, particle.y }
        , scale{ particle.size } { }

    glm::vec2 position;
    float scale;
};
#pragma pack(pop)

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
    void drawParticleLines() const;

    void createParticleVAO();
    void createParticleLinesVAO(size_t numLines);

    void updateParticleVAO() const;
    void updateParticleLinesVAO() const;

    void updateShaderModelMatrix(const Shader& shader) const;

    std::shared_ptr<const AnimationState> m_animationState;

    VAO m_particleLinesVAO;
    mutable VBO<ParticleLine> m_particleLinesVBO; //#TODO mutable
    Shader m_particleLinesShader;

    VAO m_particleVAO;
    mutable VBO<ParticleRenderData> m_particleVBO; // #TODO mutable
    Shader m_particleShader;
};

} // namespace rg
