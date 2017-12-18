#include "../headers/MainWidget.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <Windows.h>

#include "../headers/colors.h"
#include "../headers/FontManager.h"

namespace rg {

constexpr float PARTICLE_R{ 1.0f };
constexpr float PARTICLE_G{ 0.0f };
constexpr float PARTICLE_B{ 0.0f };
constexpr float PARTICLE_A{ 0.8f };

constexpr float particleSize{ 0.005f };

/* A particle that rotates around a given point */
class Particle {
public:
    Particle(float x_, float y_, float centreX, float centreY) : 
        x{ x_ }, y{ y_ }, orbitCentreX{ centreX }, orbitCentreY{ centreY }
    { }

    void draw() const {

        glPushMatrix(); {
            glTranslatef(x, y, 0.0f);
            glBegin(GL_QUADS); {
                glVertex2f(-particleSize/2.0f, particleSize/2.0f);
                glVertex2f(particleSize/2.0f, particleSize/2.0f);
                glVertex2f(particleSize/2.0f, -particleSize/2.0f);
                glVertex2f(-particleSize/2.0f, -particleSize/2.0f);
            } glEnd();
        } glPopMatrix();
    }

    float x{ 0.0f };
    float y{ 0.0f };
    float orbitCentreX{ 0.0f };
    float orbitCentreY{ 0.0f };
private:
};

void MainWidget::clear() const {
    glViewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
    scissorClear(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
}

void MainWidget::draw() const {
    if (!m_visible) return;

    clear();

    glColor4f(DIVIDER_R, DIVIDER_G, DIVIDER_B, DIVIDER_A);
    drawTopSerifLine(-1.0f, 1.0f);
    drawBottomSerifLine(-1.0f, 1.0f);

    static Particle p1{ 0.2f, -0.3f, 0.1f, -0.2f };
    static Particle p2{ -0.2f, 0.1f, 0.0f, 0.3f };

    glColor4f(PARTICLE_R, PARTICLE_G, PARTICLE_B, PARTICLE_A);
    p1.draw();
    p2.draw();

    p1.x += 0.01f;
    p1.y -= 0.01f;

    p2.x -= 0.01f;
    p2.y -= 0.01f;
}

void MainWidget::setVisibility(bool b) {
    m_visible = b;
    if (m_visible) {
        draw();
    } else {
        clear();
    }
}

}
