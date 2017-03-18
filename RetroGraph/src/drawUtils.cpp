#include "../headers/drawUtils.h"

#include <GL/glew.h>
#include <GL/gl.h>

#include "../headers/colors.h"

namespace rg {
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

void drawFilledGraph(const std::vector<float>& data) {
    glBegin(GL_QUADS); {
        glColor4f(GRAPHLINE_R, GRAPHLINE_G, GRAPHLINE_B, 0.7f);
        for (auto i = size_t{ 0U }; i < data.size() - 1; ++i) {
            const auto x1 = float{ (static_cast<float>(i) / (data.size() - 1)) * 2.0f - 1.0f };
            const auto y1 = float{ data[i] * 2.0f - 1.0f };
            const auto x2 = float{ (static_cast<float>(i+1) / (data.size() - 1)) * 2.0f - 1.0f };
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

void drawGraphGrid(GLuint graphGridVertsID, GLuint graphGridIndicesID,
                   size_t graphIndicesSize) {
    // Draw the background grid for the graph
    vboDrawScope(graphGridVertsID, graphGridIndicesID, [graphIndicesSize]() {
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