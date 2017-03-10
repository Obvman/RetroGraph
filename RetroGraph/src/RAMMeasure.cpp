#include "../headers/RAMMeasure.h"

#include <iostream>
#include <string>
#include <GL/freeglut.h>

#include "../headers/colors.h"

namespace rg {

RAMMeasure::RAMMeasure() :
    m_memStatus{},
    dataSize{ 40U },
    m_usageData{} {
}


RAMMeasure::~RAMMeasure() {
}

void RAMMeasure::init() {
    // Fill the memory stat struct with system information
    m_memStatus.dwLength = sizeof(m_memStatus);
    GlobalMemoryStatusEx(&m_memStatus);

    m_usageData.assign(dataSize, 0.0f);

    update();
}

void RAMMeasure::update() {
    GlobalMemoryStatusEx(&m_memStatus);

    // Add value to the list of load values and shift the list left
    m_usageData[0] = getLoadPercentagef();
    std::rotate(m_usageData.begin(), m_usageData.begin() + 1, m_usageData.end());
}

void RAMMeasure::draw() const {
    glPushMatrix();

    // Preserve initial viewport settings
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    // Set up the view to a portion of the screen
    /*glViewport(m_viewportStartX, m_viewportStartY,
               m_viewportWidth, m_viewportHeight);*/

    drawText();
    drawBar();
    drawGraph();

    // Reinstate previous settings
    glViewport(vp[0], vp[1], vp[2], vp[3]);

    glPopMatrix();

}

void RAMMeasure::drawBar() const {
    const auto percent = getLoadPercentagef() * 2.0f;
    constexpr float xLoc{ -0.95f };

    float lineWidth;
    glGetFloatv(GL_LINE_WIDTH, &lineWidth);

    glLineWidth(10.0f);
    glColor3f(LINE_R, LINE_G, LINE_B);
    glBegin(GL_LINES); {
        // Draw usage section
        glVertex2f(xLoc, -0.80f);
        glVertex2f(xLoc, -0.80f + percent);

        // Draw available section
        glColor3f(0.2f, 0.2f, 0.2f);
        glVertex2f(xLoc, -0.80f + percent);
        glVertex2f(xLoc,  0.80f);
    } glEnd();

    glLineWidth(lineWidth);

}

void RAMMeasure::drawText() const {
    const auto rasterX = float{ -0.957f };
    const auto rasterY = float{ 0.87f };

    auto percent = "RAM: " + std::to_string(getLoadPercentage()) + "%";

    glColor4f(TEXT_R, TEXT_G, TEXT_B, TEXT_A);
    glRasterPos2f(rasterX, rasterY);
    glCallLists(percent.length(), GL_UNSIGNED_BYTE, percent.c_str());
}

void RAMMeasure::drawGraph() const {
    GLfloat lineWidth;
    glGetFloatv(GL_LINE_WIDTH, &lineWidth);
    glLineWidth(0.5f);

    //glUseProgram(shader);
    glBegin(GL_LINE_STRIP); {
        // Draw each node in the graph
        for (auto i{ 0U }; i < m_usageData.size(); ++i) {
            glColor4f(LINE_R, LINE_G, LINE_B, 1.0f);

            float x = (static_cast<float>(i) / (m_usageData.size() - 1)) * 2.0f - 1.0f;
            float y = (m_usageData[i]) * 2.0f - 1.0f;

            // If the vertex is at the border, add/subtract the border delta
            if (i == 0) {
                x += bDelta;
            }

            glVertex3f(x, y, 0.0f);
        }
    } glEnd();

    glUseProgram(0);

    glLineWidth(lineWidth);
}

float RAMMeasure::getLoadPercentagef() const {
    return (static_cast<float>(getUsedPhysicalB()) / 
            static_cast<float>(getTotalPhysicalB()));
}

}