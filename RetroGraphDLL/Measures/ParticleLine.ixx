export module RG.Measures:ParticleLine;

import std;

import "GLHeaderUnit.h";

namespace rg {

float distance(float x1, float y1, float x2, float y2) {
    const auto dx{ fabs(x1 - x2) };
    const auto dy{ fabs(y1 - y2) };
    return dx * dx + dy * dy;
}

#pragma pack(push)
#pragma pack(1)
export struct ParticleLine {
    ParticleLine() = default;
    ParticleLine(float x1, float y1, float x2, float y2)
        : v1{ x1, y1 }
        , distance1{ distance(x1, y1, x2, y2) }
        , v2{ x2, y2 }
        , distance2{ distance1 } {}

    glm::vec2 v1;
    float distance1;

    glm::vec2 v2;
    float distance2;
};
#pragma pack(pop)

} // namespace rg
