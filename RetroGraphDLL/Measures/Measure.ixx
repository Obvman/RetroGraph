export module Measures.Measure;

import Core.CallbackEvent;
import Core.Time;

import std.core;

namespace rg {

using namespace std::chrono;

export using Hz = int;
export using Seconds = int;

export using PostUpdateEvent = CallbackEvent<>;

export enum class MeasureType : size_t {
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
    Time = 10U,

    NumMeasures,
};

export class Measure {
public:
    Measure() : m_lastUpdateTime{ steady_clock::now() } {
    }

    virtual ~Measure() = default;

    Measure(const Measure&)            = delete;
    Measure& operator=(const Measure&) = delete;
    Measure(Measure&&)                 = delete;
    Measure& operator=(Measure&&)      = delete;

    void update() {
        if (since(m_lastUpdateTime) > updateInterval()) {
            updateInternal();
            m_lastUpdateTime = steady_clock::now();
        }
    }

    PostUpdateEvent postUpdate;

protected:
    virtual void updateInternal() = 0;
    virtual std::chrono::microseconds updateInterval() const { return std::chrono::seconds{ 2 }; }

private:
    steady_clock::time_point m_lastUpdateTime;
};

}
