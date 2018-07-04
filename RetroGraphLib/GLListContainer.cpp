#include "stdafx.h"
#include "GLListContainer.h"

namespace rg {

GLListContainer::GLListContainer()
    : m_circleList{ glGenLists(1)} {

    // Create a static circle used to draw each particle.
    glNewList(m_circleList, GL_COMPILE);
    glBegin(GL_TRIANGLE_FAN); {
        glVertex2f(0.0f, 0.0f);
        for (int i = 0; i < circleLines; ++i) {
            const auto theta{ 2.0f * 3.1415926f * static_cast<float>(i) /
                static_cast<float>(circleLines - 1) };
            glVertex2f(cosf(theta), sinf(theta));
        }
    } glEnd();
    glEndList();
}


GLListContainer::~GLListContainer() {
    glDeleteLists(m_circleList, 1);
}

}
