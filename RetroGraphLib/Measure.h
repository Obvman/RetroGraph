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

    NumMeasures,
};
}

using Hz = uint32_t;
using Seconds = uint32_t;

class Measure {
public:
    Measure() = default;
    Measure(std::initializer_list<uint32_t> ur) : m_updateRates{ ur } {}
    virtual ~Measure() = default;
    Measure(const Measure&) = delete;
    Measure& operator=(const Measure&) = delete;
    Measure(Measure&&) = delete;
    Measure& operator=(Measure&&) = delete;

    virtual void update(uint32_t) = 0;

protected:
    /* Must true if the Measure should update this frame */
    virtual bool shouldUpdate(uint32_t ticks) const = 0;

    const std::vector<uint32_t> m_updateRates;
private:
    /* Stores the update rate for each measure. If key.second is true, rate 
     * is measured in Hz, otherwise measured in seconds
     */
    //static const std::map<Measures::Types, UpdateRate> updateRates;
};

}