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
    D3GPU = 9U,
    SystemInformation = 10U,
    Display = 11U,

    NumMeasures,
};
}

using Hz = uint32_t;
using Seconds = uint32_t;

class Measure {
public:
    Measure() = default;
    explicit Measure(std::initializer_list<uint32_t> ur) : m_updateRates{ ur } {}
    virtual ~Measure() = default;
    Measure(const Measure&) = delete;
    Measure& operator=(const Measure&) = delete;
    Measure(Measure&&) = delete;
    Measure& operator=(Measure&&) = delete;

    virtual void update(uint32_t) = 0;

    /* Must true if the Measure should update this frame */
    virtual bool shouldUpdate(uint32_t ticks) const = 0;
protected:

    // Updates the measure so it *should* bypass checks against ticks 
    // that may prevent an update
    inline void force_update() { update(0); }

    const std::vector<uint32_t> m_updateRates;

private:

    /* Stores the update rate for each measure. If key.second is true, rate 
     * is measured in Hz, otherwise measured in seconds
     */
    //static const std::map<Measures::Types, UpdateRate> updateRates;
};

}