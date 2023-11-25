module Widgets.MainWidget;

import Colors;
import Colors;
import Units;

import Rendering.DrawUtils;

import "GLHeaderUnit.h";

namespace rg {

MainWidget::MainWidget(const FontManager* fontManager, std::shared_ptr<const AnimationState> animationState)
    : Widget{ fontManager }
    , m_animationState{ animationState }
    , m_particleLinesVAO{}
    , m_particleLinesVBO{ GL_ARRAY_BUFFER }
    , m_particleLinesShader{ "particleLine" }
    , m_particleVAO{}
    , m_particleVBO{ GL_ARRAY_BUFFER }
    , m_particleShader{ "particle" } {

    createParticleLinesVAO(maxLines);
    createParticleVAO();
}

bool MainWidget::needsDraw(int ticks) const {
    // Only draw if visible and we need to draw to keep
    // up with the animation framerate
    if ((ticks != 0 &&
         ticks % std::lround(static_cast<float>(rg::ticksPerSecond) / m_animationState->getAnimationFPS()) != 0)) {

        return false;
    }
    return true;
}

void MainWidget::draw() const {
    // Scale by the aspect ratio of the viewport so circles aren't skewed
    const float aspectRatio = static_cast<float>(m_viewport.width) / static_cast<float>(m_viewport.height);
    glPushMatrix(); {
        glScalef(1.0f, aspectRatio, 1.0f);

        drawParticles(aspectRatio);
        drawParticleLines(aspectRatio);
    } glPopMatrix();
}

void MainWidget::reloadShaders() {
    m_particleLinesShader.reload();
    m_particleShader.reload();
}

void MainWidget::drawParticles(float aspectRatio) const {
    (void)aspectRatio;

    auto vaoScope{ m_particleVAO.bind() };
    auto vboScope{ m_particleVBO.bind() };

    glColor4f(PARTICLE_R, PARTICLE_G, PARTICLE_B, PARTICLE_A);
    for (const auto& p : m_animationState->getParticles()) {
        glPushMatrix(); {
            glTranslatef(p.x, p.y, 0.0f);
            glScalef(p.size, p.size, 1.0f);
            glDrawArrays(GL_TRIANGLE_FAN, 0, m_particleVBO.size());
        } glPopMatrix();
    }
}

void MainWidget::drawParticleLines(float aspectRatio) const {
    updateParticleLinesVAO();

    auto shaderScope{ m_particleLinesShader.bind() };
    auto vaoScope{ m_particleLinesVAO.bind() };

    glm::vec3 const scale{ glm::vec3(1.0f, aspectRatio, 1.0f) }; // #TODO only do this when aspectRatio changes.
    glm::mat4 modelMatrix{ glm::mat4() };
    modelMatrix = glm::scale(modelMatrix, scale);
    GLuint const location = m_particleLinesShader.getUniformLocation("model");
    glUniformMatrix4fv(location, 1, false, glm::value_ptr(modelMatrix));

    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_animationState->getNumLines() * 2));
}

void MainWidget::createParticleLinesVAO(size_t numLines) {
    constexpr GLuint vertexLocationIndex{ 0 };
    constexpr GLuint lineLengthLocationIndex{ 1 };

    auto vaoScope{ m_particleLinesVAO.bind() };

    m_particleLinesVBO.resize(numLines);
    auto vboScope{ m_particleLinesVBO.bind() };

    glEnableVertexAttribArray(vertexLocationIndex);
    glVertexAttribPointer(vertexLocationIndex, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    glEnableVertexAttribArray(lineLengthLocationIndex);
    glVertexAttribPointer(lineLengthLocationIndex, 1, GL_FLOAT, GL_FALSE,
                          3 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(sizeof(glm::vec2)));

    glBufferData(GL_ARRAY_BUFFER, m_particleLinesVBO.sizeBytes(), m_particleLinesVBO.data.data(), GL_STREAM_DRAW);
}

void MainWidget::createParticleVAO() {
    constexpr auto circleLines = int{ 10 };
    constexpr GLuint vertexLocationIndex{ 0 };

    auto& verts{ m_particleVBO.data };

    verts.emplace_back(0.0f, 0.0f);
    for (int i = 0; i < circleLines; ++i) {
        const auto theta{ 2.0f * 3.1415926f * static_cast<float>(i) / static_cast<float>(circleLines - 1) };
        verts.emplace_back(cosf(theta), sinf(theta));
    }

    auto vaoScope{ m_particleVAO.bind() };
    auto vboScope{ m_particleVBO.bind() };

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(vertexLocationIndex, 2, GL_FLOAT, GL_FALSE, m_particleVBO.elemBytes(), nullptr);

    glBufferData(GL_ARRAY_BUFFER, m_particleVBO.sizeBytes(), verts.data(), GL_STATIC_DRAW);
}

void MainWidget::updateParticleLinesVAO() const {
    const auto numLines{ m_animationState->getNumLines() };

    auto& verts{ m_particleLinesVBO.data };
    for (int i = 0; i < numLines; ++i) {
        verts[i] = m_animationState->getLines()[i];
    }

    auto vaoScope{ m_particleLinesVAO.bind() };
    auto vboScope{ m_particleLinesVBO.bind() };

    glBufferSubData(GL_ARRAY_BUFFER, 0, numLines * m_particleLinesVBO.elemBytes(), verts.data());
}

} // namespace rg
