export module RG.Widgets:MainWidget;

import :Widget;

import RG.Measures;
import RG.Rendering;

import std.memory;

import "GLHeaderUnit.h";

namespace rg {

#pragma pack(push)
#pragma pack(1)
struct ParticleRenderData {
    ParticleRenderData() = default;
    ParticleRenderData(const Particle& particle)
        : position{ particle.x, particle.y }
        , scale{ particle.size } {}

    glm::vec2 position;
    float scale;
};
#pragma pack(pop)

export class MainWidget : public Widget {
public:
    MainWidget(const FontManager* fontManager, std::shared_ptr<const AnimationState> animationState);
    ~MainWidget() noexcept;

    void draw() const override;

private:
    void drawParticles() const;
    void drawParticleLines() const;

    void createParticleVAO();
    void createParticleLinesVAO(size_t numLines);

    void updateParticleVAO();
    void updateParticleLinesVAO();

    void updateShaderModelMatrix(const Shader& shader) const;

    PostUpdateEvent::Handle RegisterPostUpdateCallback();

    std::shared_ptr<const AnimationState> m_animationState;
    PostUpdateEvent::Handle m_postUpdateHandle;

    VAO m_particleLinesVAO;
    VBO m_particleLinesVBO;

    VAO m_particleVAO;
    OwningVBO<ParticleRenderData> m_particleVBO;

    ShaderRefreshEvent::Handle m_onParticleShaderRefreshHandle;
    ShaderRefreshEvent::Handle m_onParticleLineShaderRefreshHandle;
};

} // namespace rg
