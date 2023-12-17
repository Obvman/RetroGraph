export module RG.Widgets:WidgetShaderController;

import RG.Rendering;

namespace rg {

export class WidgetShaderController {
public:
    static WidgetShaderController& inst() { static WidgetShaderController instance; return instance; }

    const Shader& getParticleLineShader() const { return m_particleLineShader; }
    const Shader& getParticleShader() const { return m_particleShader; }
    const Shader& getLineGraphShader() const { return m_lineGraphShader; }

private:
    WidgetShaderController() = default;

    Shader m_particleLineShader{ "particleLine" };
    Shader m_particleShader{ "particle" };
    Shader m_lineGraphShader{ "lineGraph" };
};

}
