export module RG.Core:Math;

namespace rg {

/* Linear interpolation */
export constexpr inline float lerp(float x1, float x2, float t) {
    return (1 - t) * x1 + t * x2;
}

} // namespace rg
