export module RG.Rendering:DrawUtils;

import :Viewport;

import Colors;

import std;

import "GLHeaderUnit.h";

namespace rg {

export constexpr auto viewportMin = float{ -1.0f };
export constexpr auto viewportMax = float{ 1.0f };
export constexpr auto viewportWidth = float{ 2.0f };

/* Minimum distance from the edge of the screen to draw each object (in pixels) */
export constexpr auto screenMarginX = int{ 16 };
export constexpr auto screenMarginY = int{ 10 };

export constexpr GLuint invalidGLID{ UINT_MAX };

export void scissorClear(GLint x, GLint y, GLint w, GLint h);
export void drawWidgetBackground();

/* Draws a vertical bar that is centerred horizontally in the current viewport */
export void drawVerticalProgressBar(float barWidth, float startY, float endY, float currValue, float totalValue,
                                    bool warningColor = true);

/* Draws a horizontal bar that is centerred vertically in the current viewport */
export void drawHorizontalProgressBar(float barWidth, float startX, float endX, float currValue, float totalValue);

// Primitive drawing
export void drawSerifLine(GLfloat x1, GLfloat x2, GLfloat y);

export void setGLViewport(const Viewport& vp);

export Viewport getGLViewport();

export GLenum checkGLErrors();

export constexpr inline float clampToViewport(float f) {
    return std::clamp(f, viewportMin, viewportMax);
}

// Given a percentage float value (from 0.0f -> 1.0f), this maps it into the viewport space
export constexpr inline float percentageToVP(float percentage) {
    return percentage * viewportWidth - 1.0f;
}

// Given a pixel value (x or y) and the current viewport width/height in pixels,
// converts the pixel value to the corresponding viewport ordinate
export constexpr inline float pixelsToVPCoords(int p, int vpWidthPx) {
    return percentageToVP(static_cast<float>(p) / vpWidthPx);
}

// Given a viewport ordinate (x or y) and the viewport width/height in pixels,
// converts the ordinate into a pixel value relative to the viewport origin
export constexpr inline int vpCoordsToPixels(float vpCoord, int vpWidthPx) {
    return static_cast<int>(((vpCoord + 1.0f) / viewportWidth) * vpWidthPx);
}

} // namespace rg
