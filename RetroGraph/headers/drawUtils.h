#pragma once

#include <stdint.h>
#include <vector>

#ifndef __GL_H__
typedef unsigned int GLuint;
#endif

namespace rg {

constexpr size_t VP_X{ 0U };
constexpr size_t VP_Y{ 1U };
constexpr size_t VP_WIDTH{ 2U };
constexpr size_t VP_HEIGHT{ 3U };


void drawFilledGraph(const std::vector<float>& data);
void drawLineGraph(const std::vector<float>& data);
void drawGraphGrid(GLuint graphGridVertsID, GLuint graphGridIndicesID,
                   size_t graphIndicesSize);



constexpr inline float pixelsToVPCoords(uint32_t p, uint32_t vpWidth) {
    return (static_cast<float>(p) / vpWidth) * 2.0f - 1.0f;
}

constexpr inline uint32_t vpCoordsToPixels(float vpCoord, uint32_t vpWidth) {
    return static_cast<uint32_t>(((vpCoord + 1.0f) / 2.0f) * vpWidth);
}

}
