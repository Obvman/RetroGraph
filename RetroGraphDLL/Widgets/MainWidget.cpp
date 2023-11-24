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
    , m_animVAO{}
    , m_animLines{ GL_ARRAY_BUFFER }
    , m_animParticle{ GL_ARRAY_BUFFER }
    , m_animShader{ "particleLine" } {

    createAnimationVBO(maxLines);
    createParticleVBO();
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

        drawParticles();
        drawParticleLines(aspectRatio);
    } glPopMatrix();
}

void MainWidget::reloadShaders() {
    m_animShader.reload();
}

void MainWidget::drawParticles() const {
    auto vboScope{ m_animParticle.bind() };
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, nullptr);
    glColor4f(PARTICLE_R, PARTICLE_G, PARTICLE_B, PARTICLE_A);
    for (const auto& p : m_animationState->getParticles()) {
        glPushMatrix(); {
            glTranslatef(p.x, p.y, 0.0f);
            glScalef(p.size, p.size, 1.0f);
            glDrawArrays(GL_TRIANGLE_FAN, 0, m_animParticle.size());
        } glPopMatrix();
    }
    glDisableClientState(GL_VERTEX_ARRAY);
}

void MainWidget::drawParticleLines(float aspectRatio) const {
    updateAnimationVBO();
    drawAnimationVBO(m_animationState->getNumLines() * 2, aspectRatio);
}

void MainWidget::createAnimationVBO(size_t numLines) {
    constexpr GLuint vertexLocationIndex{ 0 };
    constexpr GLuint lineLengthLocationIndex{ 1 };

    auto vaoScope{ m_animVAO.bind() };

    m_animLines.resize(numLines);
    auto vboScope{ m_animLines.bind() };

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(vertexLocationIndex, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(lineLengthLocationIndex, 1, GL_FLOAT, GL_FALSE,
                          3 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(2 * sizeof(GLfloat)));

    glBufferData(GL_ARRAY_BUFFER, m_animLines.sizeBytes(), m_animLines.data.data(), GL_STREAM_DRAW);
}

void MainWidget::createParticleVBO() {
    constexpr auto circleLines = int{ 10 };

    auto vboScope{ m_animParticle.bind() };
    auto& verts{ m_animParticle.data };

    verts.emplace_back(0.0f, 0.0f);
    for (int i = 0; i < circleLines; ++i) {
        const auto theta{ 2.0f * 3.1415926f * static_cast<float>(i) / static_cast<float>(circleLines - 1) };
        verts.emplace_back(cosf(theta), sinf(theta));
    }

    glBufferData(GL_ARRAY_BUFFER, m_animParticle.sizeBytes(), verts.data(), GL_STATIC_DRAW);
}

void MainWidget::updateAnimationVBO() const {
    const auto numLines{ m_animationState->getNumLines() };

    auto& verts{ m_animLines.data };
    for (int i = 0; i < numLines; ++i) {
        verts[i] = m_animationState->getLines()[i];
    }

    auto vaoScope{ m_animVAO.bind() };
    auto vboScope{ m_animLines.bind() };

    glBufferSubData(GL_ARRAY_BUFFER, 0, numLines * m_animLines.elemBytes(), verts.data());
}

void MainWidget::drawAnimationVBO(int size, float aspectRatio) const {
    auto shaderScope{ m_animShader.bind() };
    auto vaoScope{ m_animVAO.bind() };

    glm::vec3 const scale{ glm::vec3(1.0f, aspectRatio, 1.0f) };
    glm::mat4 modelMatrix{ glm::mat4() };
    modelMatrix = glm::scale(modelMatrix, scale);
    GLuint const location = m_animShader.getUniformLocation("model");
    glUniformMatrix4fv(location, 1, false, glm::value_ptr(modelMatrix));

    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(size));
}

} // namespace rg
