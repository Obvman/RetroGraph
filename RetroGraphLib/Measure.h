#pragma once

namespace rg {

namespace Measures {
enum Types : size_t {
    CPU = 0U,
    GPU = 1U,
    RAM = 2U,
    Net = 3U,
    Process = 4U,
    Drive = 5U,
    Music = 6U,
    System = 7U,
    AnimationState = 8U,

    NumMeasures,
};
}

class Measure {
public:
    Measure() = default;
    virtual ~Measure() = default;
    Measure(const Measure&) = delete;
    Measure& operator=(const Measure&) = delete;
    Measure(Measure&&) = delete;
    Measure& operator=(Measure&&) = delete;

    virtual void update(uint32_t) = 0;
private:
};

}