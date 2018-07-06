#include "stdafx.h"

#include "drawUtils.h"

// #include <GL/glew.h>
// #include <GL/gl.h>
#include <cmath>

#include "UserSettings.h"
#include "ListContainer.h"
#include "colors.h"

namespace rg {

void scissorClear(GLint x, GLint y, GLint w, GLint h) {
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, w, h);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
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

void drawWidgetBackground() {
    if (std::get<bool>(UserSettings::inst().getVal("Window.WidgetBackground"))) {
        ListContainer::inst().drawWidgetBackground();
    }
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

}
