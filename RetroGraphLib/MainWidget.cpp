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

MainWidget::MainWidget(const FontManager* fontManager, const RetroGraph& rg, bool visible)
    : Widget{ fontManager, visible }
    , m_animationState{ &rg.getAnimationState() }
    , m_vboID{ VBOController::inst().createAnimationVBO(maxLines) }  { }

bool MainWidget::needsDraw(int ticks) const {
    // Only draw if visible and we need to draw to keep
    // up with the animation framerate
    if (!isVisible() || 
        (ticks != 0 
        && ticks % std::lround(static_cast<float>(rg::ticksPerSecond) / m_animationState->getAnimationFPS()) != 0)) {

        return false;
    }
    return true;
}

void MainWidget::updateObservers(const RetroGraph & rg) {
    m_animationState = &rg.getAnimationState();
}

void MainWidget::draw() const {
    // Scale by the aspect ratio of the viewport so circles aren't skewed
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
    //VBOController::inst().updateAnimationVBO(m_vboID, *m_animationState);
    //VBOController::inst().drawAnimationVBO(m_vboID, m_animationState->getNumLines() * sizeof(ParticleLine));

    glBegin(GL_LINES); {
        for (int i = 0; i < m_animationState->getNumLines(); ++i) {
            const auto& line{ m_animationState->getLines()[i] };

            glColor4f(WHITE_R, WHITE_G, WHITE_B, m_animationState->getLineColours()[i]);
            glVertex2f(line.x1, line.y1);
            glVertex2f(line.x2, line.y2);
        }
    } glEnd();
}

}
