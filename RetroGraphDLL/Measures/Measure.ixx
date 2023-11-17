export module Measures.Measure;

import Utils;

import std.core;

namespace rg {

export using Hz = int;
export using Seconds = int;

namespace Measures {

    export enum class Types : size_t {
        CPU = 0U,
        GPU = 1U,
        RAM = 2U,
        Net = 3U,
        Process = 4U,
        Drive = 5U,
        Music = 6U,
        System = 7U,
        AnimationState = 8U,
        Display = 9U,

        NumMeasures,
    };

}

export class Measure {
public:
    Measure() = default;
    virtual ~Measure() = default;

    Measure(const Measure&)            = delete;
    Measure& operator=(const Measure&) = delete;
    Measure(Measure&&)                 = delete;
    Measure& operator=(Measure&&)      = delete;

    virtual void update(int) = 0;

    /* Must true if the Measure should update this frame 
     * By default, most measure update every 2 seconds.
     */
    virtual bool shouldUpdate(int ticks) const { return ticksMatchRate(ticks, 2); }

    virtual void refreshSettings() = 0;
protected:

    // Updates the measure so it *should* bypass checks against ticks 
    // that may prevent an update
    inline void force_update() { update(0); }
};

}
