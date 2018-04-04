#include "../headers/MainWidget.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <Windows.h>

#include "../headers/colors.h"
#include "../headers/units.h"
#include "../headers/utils.h"
#include "../headers/FontManager.h"
#include "../headers/AnimationState.h"

namespace rg {

void MainWidget::clear() const {
    glViewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
    scissorClear(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
}

void MainWidget::draw(uint32_t ticks) const {
    if (!m_visible) {
        return;
    }

    // Only draw if ticks are 0, or we have to draw this
    // frame to maintain our FPS
    if (ticks != 0 &&
        ticks % std::lround(
            static_cast<float>(rg::ticksPerSecond) /
            m_animationState->getAnimationFPS()) != 0) {
        return;
    }

    clear();

    drawWidgetBackground();

    glColor4f(DIVIDER_R, DIVIDER_G, DIVIDER_B, DIVIDER_A);
    drawTopSerifLine(-1.0f, 1.0f);
    drawBottomSerifLine(-1.0f, 1.0f);

    glColor4f(PARTICLE_R, PARTICLE_G, PARTICLE_B, PARTICLE_A);

    float aspect = static_cast<float>(m_viewport.width) /
                   static_cast<float>(m_viewport.height);
    glPushMatrix();
    glScalef(1.0f, aspect, 1.0f);
        m_animationState->drawParticles();
    glPopMatrix();
}

void MainWidget::setVisibility(bool b) {
    m_visible = b;
    if (m_visible) {
        draw(0);
    } else {
        clear();
    }
}

}
