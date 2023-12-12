export module Measures.Measure;

import Core.Time;

import std.core;

import "EventppHeaderUnit.h";

namespace rg {

using namespace std::chrono;

export using Hz = int;
export using Seconds = int;

export using PostUpdateCallbackList = eventpp::CallbackList<void()>;
export using PostUpdateCallbackHandle = PostUpdateCallbackList::Handle;

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

    NumMeasures,
};

export class Measure {
public:
    Measure()
        : m_lastUpdateTime{ steady_clock::now() } {

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

    mutable PostUpdateCallbackList postUpdate;

protected:
    virtual void updateInternal() = 0;
    virtual std::chrono::microseconds updateInterval() const { return std::chrono::seconds{ 2 }; }

private:
    steady_clock::time_point m_lastUpdateTime;
};

}
