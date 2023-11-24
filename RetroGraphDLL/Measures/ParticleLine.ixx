export module Measures.ParticleLine;

import std.core;

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
    ParticleLine(float x1_, float y1_, float x2_, float y2_)
        : x1{ x1_ }
        , y1{ y1_ }
        , distance1{ distance(x1_, y1_, x2_, y2_) }
        , x2{ x2_ }
        , y2{ y2_ }
        , distance2{ distance1 }
    { }

    float x1;
    float y1;
    float distance1;

    float x2;
    float y2;
    float distance2;
};
#pragma pack(pop)

} // namespace rg