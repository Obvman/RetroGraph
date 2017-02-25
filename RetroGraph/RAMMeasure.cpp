#include "RAMMeasure.h"

#include <string>
#include <GL/freeglut.h>

#include "colors.h"

namespace rg {

RAMMeasure::RAMMeasure(GLint startX, GLint startY, GLint ramWidth, GLint ramHeight) :
    m_memStatus{},
    m_viewportStartX{ startX },
    m_viewportStartY{ startY },
    m_viewportWidth{ ramWidth },
    m_viewportHeight{ ramHeight } {

    // Fill the memory stat struct with system information
    m_memStatus.dwLength = sizeof(m_memStatus);
    GlobalMemoryStatusEx(&m_memStatus);

}


RAMMeasure::~RAMMeasure() {
}

void RAMMeasure::update() {
    GlobalMemoryStatusEx(&m_memStatus);
}

void RAMMeasure::draw() const {
    glPushMatrix();

    // Preserve initial viewport settings
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    // Set up the view to a portion of the screen
    glViewport(m_viewportStartX, m_viewportStartY,
               m_viewportWidth, m_viewportHeight);

    drawText();
    drawBar();
    //drawViewportBorder();

    // Reinstate previous settings
    glViewport(vp[0], vp[1], vp[2], vp[3]);

    glPopMatrix();

}

void RAMMeasure::drawBar() const {
    auto percent = (static_cast<float>(getLoadPercentage()) / 100.0f) * 2.0f;

    float lineWidth;
    glGetFloatv(GL_LINE_WIDTH, &lineWidth);

    glLineWidth(10.0f);
    glColor3f(LINE_R, LINE_G, LINE_B);
    glBegin(GL_LINES); {
        // Draw usage section
        glVertex2f(-0.95f, 0.0f);
        glVertex2f(-0.95f + percent, 0.0f);

        // Draw available section
        glColor3f(0.2f, 0.2f, 0.2f);
        glVertex2f(-0.95f + percent, 0.0f);
        glVertex2f(0.95f, 0.0f);
    } glEnd();

    glLineWidth(lineWidth);

}

void RAMMeasure::drawText() const {
    const auto rasterX = float{ -0.95f };
    const auto rasterY = float{ 0.1f };

    auto percent = "RAM: " + std::to_string(getLoadPercentage()) + "%";

    glColor3f(TEXT_R, TEXT_G, TEXT_B);
    glRasterPos2f(rasterX, rasterY);
    /*for (const auto c : percent) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }*/
    glCallLists(percent.length(), GL_UNSIGNED_BYTE, percent.c_str());
}

}