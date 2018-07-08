#include "stdafx.h"

#include "MainWidget.h"

#include <iostream>

// #include <GL/glew.h>
// #include <GL/gl.h>
#include <Windows.h>

#include "colors.h"
#include "units.h"
#include "utils.h"
#include "FontManager.h"
#include "AnimationState.h"
#include "RetroGraph.h"
#include "ListContainer.h"

namespace rg {

bool MainWidget::needsDraw(uint32_t ticks) const {
    // Only draw if visible and we need to draw to keep
    // up with the animation framerate
    if (!isVisible() ||
        (ticks != 0 &&
        ticks % std::lround(
            static_cast<float>(rg::ticksPerSecond) /
            m_animationState->getAnimationFPS()) != 0)) {

        return false;
    }
    return true;
}

void MainWidget::updateObservers(const RetroGraph & rg) {
    m_animationState = &rg.getAnimationState();
}

void MainWidget::draw() const {
    clear();

    drawWidgetBackground();

    ListContainer::inst().drawTopAndBottomSerifs();

    glColor4f(PARTICLE_R, PARTICLE_G, PARTICLE_B, PARTICLE_A);

    float aspect = static_cast<float>(m_viewport.width) /
                   static_cast<float>(m_viewport.height);
    glPushMatrix(); {
        glScalef(1.0f, aspect, 1.0f);
        drawParticles();
        drawParticleLines();
    } glPopMatrix();
}

void MainWidget::drawParticles() const {
    for (const auto& p : m_animationState->getParticles()) {
        glPushMatrix(); {
            glTranslatef(p.x, p.y, 0.0f);
            glScalef(p.size, p.size, 1.0f);
            ListContainer::inst().drawCircle();
        } glPopMatrix();
    }
}

void MainWidget::drawParticleLines() const {
    glBegin(GL_LINES); {
        for (int i = 0; i < m_animationState->getNumLines(); ++i) {
            const auto& line{ m_animationState->getLine(i) };

            glColor4f(WHITE_R, WHITE_G, WHITE_B, line.alpha);
            glVertex2f(line.x1, line.y1);
            glVertex2f(line.x2, line.y2);
        }
    } glEnd();
}

}
