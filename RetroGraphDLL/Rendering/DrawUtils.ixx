export module Rendering.DrawUtils;

import Colors;

import Rendering.Viewport;

import std.core;

import "GLHeaderUnit.h";

namespace rg {

/* Minimum distance from the edge of the screen to draw each object (in pixels) */
export constexpr auto marginX = int{ 16 };
export constexpr auto marginY = int{ 10 };

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

export void setGLViewport(const Viewport& vp);

export Viewport getGLViewport();

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

}