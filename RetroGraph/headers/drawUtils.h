#pragma once

#include <stdint.h>
#include <vector>

#ifndef __GL_H__
typedef unsigned int GLuint;
typedef int GLint;
#endif

namespace rg {

constexpr size_t VP_X{ 0U };
constexpr size_t VP_Y{ 1U };
constexpr size_t VP_WIDTH{ 2U };
constexpr size_t VP_HEIGHT{ 3U };

struct Viewport {
    GLint x;
    GLint y;
    GLint width;
    GLint height;

    Viewport() {}
    Viewport(GLint* vals) : x{ vals[0] }, y{ vals[1] },
                            width{vals[2]}, height{ vals[3] } {}
    void set(GLint _x, GLint _y, GLint _width, GLint _height) {
        x = _x; y = _y; width = _width; height = _height;
    }
};


void drawFilledGraph(const std::vector<float>& data);
void drawLineGraph(const std::vector<float>& data);
void drawGraphGrid(GLuint graphGridVertsID, GLuint graphGridIndicesID,
                   size_t graphIndicesSize);
void drawBorder();

constexpr inline float pixelsToVPCoords(uint32_t p, uint32_t vpWidth) {
    return (static_cast<float>(p) / vpWidth) * 2.0f - 1.0f;
}

constexpr inline uint32_t vpCoordsToPixels(float vpCoord, uint32_t vpWidth) {
    return static_cast<uint32_t>(((vpCoord + 1.0f) / 2.0f) * vpWidth);
}

}
