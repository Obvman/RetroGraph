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

    glColor4f(DIVIDER_R, DIVIDER_G, DIVIDER_B, DIVIDER_A);
    ListContainer::inst().drawTopAndBottomSerifs();

    glColor4f(PARTICLE_R, PARTICLE_G, PARTICLE_B, PARTICLE_A);

    float aspect = static_cast<float>(m_viewport.width) /
                   static_cast<float>(m_viewport.height);
    glPushMatrix();
    glScalef(1.0f, aspect, 1.0f);
        m_animationState->drawParticles();
    glPopMatrix();
}

}
