module Widgets.MainWidget;

import Colors;
import Units;

import Rendering.DrawUtils;

import Widgets.WidgetShaderController;

import "GLHeaderUnit.h";

namespace rg {

MainWidget::MainWidget(const FontManager* fontManager, std::shared_ptr<const AnimationState> animationState)
    : Widget{ fontManager }
    , m_animationState{ animationState }
    , m_postUpdateHandle{ RegisterPostUpdateCallback() }
    , m_particleLinesVAO{}
    , m_particleLinesVBO{ GL_ARRAY_BUFFER, GL_STREAM_DRAW }
    , m_particleVAO{}
    , m_particleVBO{ static_cast<GLsizei>(m_animationState->getParticles().size()), GL_ARRAY_BUFFER, GL_STREAM_DRAW } {

    createParticleLinesVAO(maxLines);
    createParticleVAO();

    updateShaderModelMatrix(WidgetShaderController::inst().getParticleShader());
    updateShaderModelMatrix(WidgetShaderController::inst().getParticleLineShader());
}

MainWidget::~MainWidget() {
    m_animationState->postUpdate.remove(m_postUpdateHandle);
}

bool MainWidget::needsDraw(int ticks) const {
    // Only draw if we need to draw to keep // up with the animation framerate
    if ((ticks != 0 &&
         ticks % std::lround(static_cast<float>(rg::ticksPerSecond) / m_animationState->getAnimationFPS()) != 0)) {

        return false;
    }
    return true;
}

void MainWidget::draw() const {
    drawParticleLines();
    drawParticles();
}

void MainWidget::reloadShaders() {
    updateShaderModelMatrix(WidgetShaderController::inst().getParticleShader());
    updateShaderModelMatrix(WidgetShaderController::inst().getParticleLineShader());
}

void MainWidget::drawParticles() const {
    auto shaderScope{ WidgetShaderController::inst().getParticleShader().bind() };
    auto vaoScope{ m_particleVAO.bind() };

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    glDrawArrays(GL_POINTS, 0, m_particleVBO.size());
}

void MainWidget::drawParticleLines() const {
    auto shaderScope{ WidgetShaderController::inst().getParticleLineShader().bind() };
    auto vaoScope{ m_particleLinesVAO.bind() };

    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_animationState->getNumLines() * 2));
}

void MainWidget::createParticleVAO() {
    constexpr GLuint vertexLocationIndex{ 0 };
    constexpr GLuint scaleLocationIndex{ 1 };

    auto& verts{ m_particleVBO.data() };
    for (int i{ 0 }; i < m_animationState->getParticles().size(); ++i) {
        verts[i] = m_animationState->getParticles()[i];
    }

    auto vaoScope{ m_particleVAO.bind() };
    auto vboScope{ m_particleVBO.bind() };

    glEnableVertexAttribArray(vertexLocationIndex);
    glVertexAttribPointer(vertexLocationIndex, 2, GL_FLOAT, GL_FALSE, m_particleVBO.elemBytes(), nullptr);

    glEnableVertexAttribArray(scaleLocationIndex);
    glVertexAttribPointer(scaleLocationIndex, 1, GL_FLOAT, GL_FALSE, m_particleVBO.elemBytes(),
                          reinterpret_cast<GLvoid*>(sizeof(glm::vec2)));

    m_particleVBO.bufferData();
}

void MainWidget::createParticleLinesVAO(size_t numLines) {
    constexpr GLuint vertexLocationIndex{ 0 };
    constexpr GLuint lineLengthLocationIndex{ 1 };

    auto vaoScope{ m_particleLinesVAO.bind() };
    auto vboScope{ m_particleLinesVBO.bind() };

    glEnableVertexAttribArray(vertexLocationIndex);
    glVertexAttribPointer(vertexLocationIndex, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    glEnableVertexAttribArray(lineLengthLocationIndex);
    glVertexAttribPointer(lineLengthLocationIndex, 1, GL_FLOAT, GL_FALSE,
                          3 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(sizeof(glm::vec2)));

    m_particleLinesVBO.bufferData(numLines * sizeof(ParticleLine), m_animationState->getLines().data());
}

void MainWidget::updateParticleVAO() {
    auto& verts{ m_particleVBO.data() };
    for (int i{ 0 }; i < m_animationState->getParticles().size(); ++i) {
        const auto& particle{ m_animationState->getParticles()[i] };
        verts[i].position = { particle.x, particle.y };
    }

    auto vaoScope{ m_particleVAO.bind() };
    auto vboScope{ m_particleVBO.bind() };

    m_particleVBO.bufferSubData(0,  m_particleVBO.sizeBytes());
}

void MainWidget::updateParticleLinesVAO() {
    auto vaoScope{ m_particleLinesVAO.bind() };
    auto vboScope{ m_particleLinesVBO.bind() };

    m_particleLinesVBO.bufferSubData(0, m_animationState->getNumLines() * sizeof(ParticleLine),
                                     m_animationState->getLines().data());
}

void MainWidget::updateShaderModelMatrix(const Shader& shader) const {
    auto shaderScope{ shader.bind() };

    // Draw things slightly larger than the viewport so it's not as jarring when particles go past
    // the edge and lines disappear
    glm::mat4 modelMatrix{};
    modelMatrix = glm::scale(modelMatrix, { 1.2f, 1.2f, 1.0f });
    glUniformMatrix4fv(shader.getUniformLocation("model"), 1, false, glm::value_ptr(modelMatrix));
}

PostUpdateCallbackHandle MainWidget::RegisterPostUpdateCallback() {
    return m_animationState->postUpdate.append(
        [this]() {
            updateParticleVAO();
            updateParticleLinesVAO();
        });
}

} // namespace rg
