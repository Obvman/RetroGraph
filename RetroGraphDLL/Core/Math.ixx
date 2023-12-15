export module Core.Math;

namespace rg {

/* Linear interpolation */
export __declspec(dllexport) constexpr inline float lerp(float x1, float x2, float t) {
    return (1 - t) * x1 + t * x2;
}

}
