#pragma once

#include "stdafx.h"

#include <cstdint>
#include <vector>

#ifndef __GL_H__
typedef float GLfloat;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef int GLint;
#endif

namespace rg {

constexpr auto circleLines = int{ 10 };

constexpr auto serifLen = float{ 0.05f };

/* Minimum distance from the edge of the screen to draw each object (in pixels) */
constexpr auto marginX = int{ 16 };
constexpr auto marginY = int{ 10 };

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

void scissorClear(GLint x, GLint y, GLint w, GLint h);

void drawFilledGraph(const std::vector<float>& data);
void drawLineGraph(const std::vector<float>& data);
void drawWidgetBackground();

/* Draws a vertical bar that is centerred horizontally in the current viewport */
void drawVerticalProgressBar(float barWidth, float startY, float endY,
                             float currValue, float totalValue, 
                             bool warningColor = true);

/* Draws a horizontal bar that is centerred vertically in the current viewport */
void drawHorizontalProgressBar(float barWidth, float startX, float endX,
                               float currValue, float totalValue);

// Primitive drawing
void drawSerifLine(GLfloat x1, GLfloat x2, GLfloat y);

// Given a pixel value (x or y) and the current viewport width/height in pixels,
// converts the pixel value to the corresponding viewport ordinate
constexpr inline float pixelsToVPCoords(int p, int vpWidth) {
    return (static_cast<float>(p) / vpWidth) * 2.0f - 1.0f;
}

// Given a viewport ordinate (x or y) and the viewport width/height in pixels,
// converts the ordinate into a pixel value relative to the viewport origin
constexpr inline int vpCoordsToPixels(float vpCoord, int vpWidth) {
    return static_cast<int>(((vpCoord + 1.0f) / 2.0f) * vpWidth);
}

} // namespace rg
