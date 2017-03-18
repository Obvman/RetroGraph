#pragma once

#include <stdint.h>
#include <vector>

#ifndef __GL_H__
typedef unsigned int GLuint;
#endif

namespace rg {

void drawFilledGraph(const std::vector<float>& data);
void drawLineGraph(const std::vector<float>& data);
void drawGraphGrid(GLuint graphGridVertsID, GLuint graphGridIndicesID,
                   size_t graphIndicesSize);



constexpr float pixelsToVPCoords(uint32_t p, uint32_t vpWidth) {
    return (static_cast<float>(p) / vpWidth) * 2.0f - 1.0f;
}

constexpr uint32_t vpCoordsToPixels(float vpCoord, uint32_t vpWidth) {
    return static_cast<uint32_t>(((vpCoord + 1.0f) / 2.0f) * vpWidth);
}

}
