module;

export module DrawUtils;

import Colors;
import GLListContainer;
import Units;
import UserSettings;

import <cmath>;
import <vector>;

import <GLHeaders.h>;
import <WindowsHeaders.h>;

namespace rg {

/* Minimum distance from the edge of the screen to draw each object (in pixels) */
export constexpr auto marginX = int{ 16 };
export constexpr auto marginY = int{ 10 };

export {
    struct Viewport;
}

export void scissorClear(GLint x, GLint y, GLint w, GLint h);
export void drawFilledGraph(const std::vector<float>& data);
export void drawLineGraph(const std::vector<float>& data);
export void drawWidgetBackground();

/* Draws a vertical bar that is centerred horizontally in the current viewport */
export void drawVerticalProgressBar(float barWidth, float startY, float endY,
                             float currValue, float totalValue, 
                             bool warningColor = true);

/* Draws a horizontal bar that is centerred vertically in the current viewport */
export void drawHorizontalProgressBar(float barWidth, float startX, float endX,
                               float currValue, float totalValue);

// Primitive drawing
export void drawSerifLine(GLfloat x1, GLfloat x2, GLfloat y);

export void viewport(const Viewport& vp);

// Given a pixel value (x or y) and the current viewport width/height in pixels,
// converts the pixel value to the corresponding viewport ordinate
export constexpr inline float pixelsToVPCoords(int p, int vpWidth) {
    return (static_cast<float>(p) / vpWidth) * 2.0f - 1.0f;
}

// Given a viewport ordinate (x or y) and the viewport width/height in pixels,
// converts the ordinate into a pixel value relative to the viewport origin
export constexpr inline int vpCoordsToPixels(float vpCoord, int vpWidth) {
    return static_cast<int>(((vpCoord + 1.0f) / 2.0f) * vpWidth);
}

struct Viewport {
    GLint x{ 0 };
    GLint y{ 0 };
    GLint width{ 0 };
    GLint height{ 0 };

    Viewport() = default;
    Viewport(GLint _x, GLint _y, GLint _width, GLint _height) :
        x{ _x }, y{ _y }, width{ _width }, height{ _height } {}
    void set(GLint _x, GLint _y, GLint _width, GLint _height) {
        x = _x; y = _y; width = _width; height = _height;
    }
};


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
    if (UserSettings::inst().getVal<bool>("Window.WidgetBackground")) {
        GLListContainer::inst().drawWidgetBackground();
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

void viewport(const Viewport& vp) {
    glViewport(vp.x, vp.y, vp.width, vp.height);
}

}