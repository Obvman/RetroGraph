module Widgets.MainWidget;

import Colors;
import Colors;
import Units;

import Rendering.DrawUtils;
import Rendering.GLListContainer;
import Rendering.VBOController;

import "GLHeaderUnit.h";

namespace rg {

MainWidget::MainWidget(const FontManager* fontManager, std::shared_ptr<const AnimationState> animationState)
    : Widget{ fontManager }
    , m_animationState{ animationState } {

    VBOController::inst().createAnimationVBO(maxLines);
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

void MainWidget::drawParticles() const {
    for (const auto& p : m_animationState->getParticles()) {
        glPushMatrix(); {
            glTranslatef(p.x, p.y, 0.0f);
            glScalef(p.size, p.size, 1.0f);
            GLListContainer::inst().drawCircle();
        } glPopMatrix();
    }
}

void MainWidget::drawParticleLines(float aspectRatio) const {
    VBOController::inst().updateAnimationVBO(*m_animationState);
    VBOController::inst().drawAnimationVBO(m_animationState->getNumLines() * 2, aspectRatio);
}

} // namespace rg
