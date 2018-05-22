#pragma once

namespace rg {

namespace Measures {
enum Types : size_t {
    CPUMeasure = 0U,
    GPUMeasure = 1U,
    RAMMeasure = 2U,
    NetMeasure = 3U,
    ProcessMeasure = 4U,
    DriveMeasure = 5U,
    MusicMeasure = 6U,
    SystemMeasure = 7U,
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