export module Measures.ParticleLine;

namespace rg {

export struct ParticleLine {
    ParticleLine() = default;
    ParticleLine(float x1_, float y1_, float x2_, float y2_)
        : x1{ x1_ }
        , y1{ y1_ }
        , x2{ x2_ }
        , y2{ y2_ } { }
    ~ParticleLine() = default;

    float x1;
    float y1;
    float x2;
    float y2;
};

} // namespace rg