#include "stdafx.h"

#include "drawUtils.h"

// #include <GL/glew.h>
// #include <GL/gl.h>
#include <cmath>

#include "UserSettings.h"
#include "colors.h"

namespace rg {

GLuint graphGridVertsID;
GLuint graphGridIndicesID;
GLsizei graphIndicesSize;

// Automatically binds/unbinds given VBOs and executes the function given
template<typename F>
void vboDrawScope(GLuint vertID, GLuint indexID, F f) {
    glBindBuffer(GL_ARRAY_BUFFER, vertID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);

    f();

    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void scissorClear(GLint x, GLint y, GLint w, GLint h) {
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, w, h);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}

void drawCircle(GLfloat x, GLfloat y, GLfloat radius) {
    constexpr float dToR{ 3.151592f / 180.0f };

    glBegin(GL_LINE_LOOP); {
        for (auto i = size_t{ 0U }; i < 20; ++i) {
            const auto degreesR = float{ i * dToR };
            glVertex2f(x + std::cos(degreesR) * radius,
                       y + std::sin(degreesR) * radius);
        }
    } glEnd();
}

void drawFilledGraph(const std::vector<float>& data) {
    glBegin(GL_QUADS); {
        glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.7f);
        for (auto i = size_t{ 0U }; i < data.size() - 1; ++i) {
            const auto x1 = float{ (static_cast<float>(i) / (data.size() - 1)) *
                                   2.0f - 1.0f };
            const auto y1 = float{ data[i] * 2.0f - 1.0f };
            const auto x2 = float{ (static_cast<float>(i+1) / (data.size() - 1)) *
                                   2.0f - 1.0f };
            const auto y2 = float{ data[i+1] * 2.0f - 1.0f };

            glVertex2f(x1, -1.0f); // Bottom-left
            glVertex2f(x1, y1); // Top-left
            glVertex2f(x2, y2); // Top-right
            glVertex2f(x2, -1.0f); // Bottom-right
        }
    } glEnd();
}

void drawLineGraph(const std::vector<float>& data) {
    glBegin(GL_LINE_STRIP); {
        glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, GRAPHLINE_A);
        // Draw each node in the graph
        for (auto i = size_t{ 0U }; i < data.size(); ++i) {

            const auto x = float{ (static_cast<float>(i) / (data.size() - 1))
                                   * 2.0f - 1.0f };
            const auto y = float{ data[i] * 2.0f - 1.0f };

            glVertex2f(x, y);
        }
    } glEnd();
}

void drawGraphGrid() {
    // Draw the background grid for the graph
    vboDrawScope(graphGridVertsID, graphGridIndicesID, []() {
        glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.2f);
        glLineWidth(0.5f);
        glDrawElements(GL_LINES, graphIndicesSize, GL_UNSIGNED_INT, 0);
    });
}

void drawBorder() {
    glColor4f(BORDER_R, BORDER_G, BORDER_B, BORDER_A);
    glLineWidth(0.5f);
    glBegin(GL_LINE_STRIP); {
        glVertex2f(-1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(-1.0f, 1.0f);
    } glEnd();
}

void drawWidgetBackground() {
    if (std::get<bool>(UserSettings::inst().getVal("Window.WidgetBackground"))) {
        glColor4f(WBG_R, WBG_G, WBG_B, WBG_A);
        glBegin(GL_QUADS); {
            glVertex2f(-1.0f,  1.0f);
            glVertex2f( 1.0f,  1.0f);
            glVertex2f( 1.0f, -1.0f);
            glVertex2f(-1.0f, -1.0f);
        } glEnd();
    }
}

void drawViewportBorder() {
    float color[4];
    glGetFloatv(GL_CURRENT_COLOR, color);

    glColor3f(DEBUG_BORDER_R, DEBUG_BORDER_G, DEBUG_BORDER_B);

    // Preserve initial line width
    GLfloat lineWidth;
    glGetFloatv(GL_LINE_WIDTH, &lineWidth);

    glLineWidth(5.0f);

    glBegin(GL_LINES); {
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(-1.0f,  1.0f);

        glVertex2f(-1.0f,  1.0f);
        glVertex2f( 1.0f,  1.0f);

        glVertex2f( 1.0f,  1.0f);
        glVertex2f( 1.0f, -1.0f);

        glVertex2f( 1.0f, -1.0f);
        glVertex2f(-1.0f, -1.0f);
    } glEnd();

    glColor4f(color[0], color[1], color[2], color[3]);
    glLineWidth(lineWidth);
}

void drawVerticalProgressBar(float barWidth, float startY, float endY,
                             float currValue, float totalValue, 
                             bool warningColor) {
    const auto percentage{ currValue / totalValue };
    const auto startX = float{ ((2.0f - barWidth) / 2.0f) - 1.0f };
    const auto rangeY{ endY - startY };

    glBegin(GL_QUADS); {
        glColor3f(BARFILLED_R, BARFILLED_G, BARFILLED_B);
        glVertex2f(startX + barWidth, startY);
        glVertex2f(startX, startY);
        // Draw high percentages in warning colour gradient
        if (warningColor && percentage > 0.8f) {
            glColor4f(WARNING_BAR_R, WARNING_BAR_G, WARNING_BAR_B, 0.4f);
        }
        glVertex2f(startX, startY + percentage * rangeY);
        glVertex2f(startX + barWidth, startY + percentage * rangeY);

        glColor3f(BARFREE_R, BARFREE_G, BARFREE_B);
        glVertex2f(startX, startY + percentage * rangeY);
        glVertex2f(startX, endY);
        glVertex2f(startX + barWidth, endY);
        glVertex2f(startX + barWidth, startY + percentage * rangeY);
    } glEnd();
}

void drawHorizontalProgressBar(float barWidth, float startX, float endX,
                               float currValue, float totalValue) {

    const auto percentage{ currValue / totalValue };
    const auto barStartY = float{ ((2.0f - barWidth) / 2.0f) - 1.0f };
    const auto rangeX{ endX - startX };

    glBegin(GL_QUADS); {
        glColor3f(BARFILLED_R, BARFILLED_G, BARFILLED_B);
        glVertex2f(startX,                       barStartY);
        glVertex2f(startX,                       barStartY + barWidth);
        glVertex2f(startX + percentage * rangeX, barStartY + barWidth);
        glVertex2f(startX + percentage * rangeX, barStartY);

        glColor3f(BARFREE_R, BARFREE_G, BARFREE_B);
        glVertex2f(startX + percentage * rangeX, barStartY);
        glVertex2f(startX + percentage * rangeX, barStartY + barWidth);
        glVertex2f(endX,                         barStartY + barWidth);
        glVertex2f(endX,                         barStartY);
    } glEnd();
}

void drawSerifLine(GLfloat x1, GLfloat x2, GLfloat y) {
    glBegin(GL_LINES); {
        glVertex2f(x1, y);
        glVertex2f(x2, y);

        glVertex2f(x1, y-serifLen);
        glVertex2f(x1, y+serifLen);

        glVertex2f(x2, y-serifLen);
        glVertex2f(x2, y+serifLen);
    } glEnd();
}

void drawTopSerifLine(GLfloat x1, GLfloat x2) {
    glBegin(GL_LINES); {
        glVertex2f(x1, 1.0f);
        glVertex2f(x2, 1.0f);

        glVertex2f(x1, 1.0f-serifLen);
        glVertex2f(x1, 1.0f);

        glVertex2f(x2, 1.0f-serifLen);
        glVertex2f(x2, 1.0f);
    } glEnd();
}

void drawBottomSerifLine(GLfloat x1, GLfloat x2) {
    glBegin(GL_LINES); {
        glVertex2f(x1, -1.0f);
        glVertex2f(x2, -1.0f);

        glVertex2f(x1, -1.0f);
        glVertex2f(x1, -1.0f + serifLen);

        glVertex2f(x2, -1.0f);
        glVertex2f(x2, -1.0f + serifLen);
    } glEnd();
}

}
