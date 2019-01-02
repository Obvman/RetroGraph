#pragma once

#include <map>

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
    SystemInformation = 9U,
    Display = 10U,

    NumMeasures,
};
}

using Hz = int;
using Seconds = int;

class Measure {
public:
    Measure() = default;
    explicit Measure(std::initializer_list<int> ur) : m_updateRates{ ur } {}
    virtual ~Measure() = default;

    Measure(const Measure&)            = delete;
    Measure& operator=(const Measure&) = delete;
    Measure(Measure&&)                 = delete;
    Measure& operator=(Measure&&)      = delete;

    virtual void update(int) = 0;

    /* Must true if the Measure should update this frame */
    virtual bool shouldUpdate(int ticks) const = 0;

    virtual void refreshSettings() = 0;
protected:

    // Updates the measure so it *should* bypass checks against ticks 
    // that may prevent an update
    inline void force_update() { update(0); }

    std::vector<int> m_updateRates;
};

}