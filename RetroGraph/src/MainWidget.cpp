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

    // glMatrixMode(GL_PROJECTION);
    // glPushMatrix();
    // glLoadIdentity();
    // float aspect = static_cast<float>(m_viewport.width) / static_cast<float>(m_viewport.height);
    // const float l = m_viewport.width;
    // const float r = (float)m_viewport.width + m_viewport.x;
    // const float t = (float)m_viewport.height + m_viewport.y;
    // const float b = m_viewport.y;
    // printf("Left: %f, Right: %f, Bottom: %f, Top: %f\n", l, r, b, t);
    // glOrtho(l, r, b, t, 0, 1);
    // printf("Left: %d, Right: %d, Bottom: %d, Top: %d\n", m_viewport.x, m_viewport.x + m_viewport.width,
           // m_viewport.y, m_viewport.y + m_viewport.height);
    // glOrtho(m_viewport.x, m_viewport.x + m_viewport.width, m_viewport.y, m_viewport.y + m_viewport.height, 0.0f, 1.0f);

    // glMatrixMode(GL_MODELVIEW);
    // printTimeToExecuteHighRes("Animation Draw", [this]() {
        m_animationState->drawParticles();
    // });

    // glMatrixMode(GL_PROJECTION);
    // glPopMatrix();
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
