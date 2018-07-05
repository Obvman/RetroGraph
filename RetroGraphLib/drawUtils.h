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

constexpr auto circleLines = int32_t{ 10 };

constexpr float serifLen{ 0.05f };

/* Minimum distance from the edge of the screen to draw each object (in pixels) */
constexpr int32_t marginX{ 16 };
constexpr int32_t marginY{ 10 };

// TODO remove these globals
extern GLuint graphGridVertsID;
extern GLuint graphGridIndicesID;
extern GLsizei graphIndicesSize;

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
void drawGraphGrid();
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
void drawTopSerifLine(GLfloat x1, GLfloat x2);
void drawBottomSerifLine(GLfloat x1, GLfloat x2);

// Given a pixel value (x or y) and the current viewport width/height in pixels,
// converts the pixel value to the corresponding viewport ordinate
constexpr inline float pixelsToVPCoords(uint32_t p, uint32_t vpWidth) {
    return (static_cast<float>(p) / vpWidth) * 2.0f - 1.0f;
}

// Given a viewport ordinate (x or y) and the viewport width/height in pixels,
// converts the ordinate into a pixel value relative to the viewport origin
constexpr inline uint32_t vpCoordsToPixels(float vpCoord, uint32_t vpWidth) {
    return static_cast<uint32_t>(((vpCoord + 1.0f) / 2.0f) * vpWidth);
}

} // namespace rg
