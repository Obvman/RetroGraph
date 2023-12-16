export module RG.Measures:Measure;

import RG.Core;

import std.core;

namespace rg {

using namespace std::chrono;

export using PostUpdateEvent = CallbackEvent<>;

export class Measure {
public:
    Measure() : m_lastUpdateTime{ steady_clock::now() } { }

    virtual ~Measure() = default;

    Measure(const Measure&)            = delete;
    Measure& operator=(const Measure&) = delete;
    Measure(Measure&&)                 = delete;
    Measure& operator=(Measure&&)      = delete;

    void update() {
        if (since(m_lastUpdateTime) > updateInterval()) {
            if (updateInternal()) {
                postUpdate.raise();
            }
            m_lastUpdateTime = high_resolution_clock::now();
        }
    }

    PostUpdateEvent postUpdate;

protected:
    // The implementation of measure updates. Returns true if any data was modified, otherwise returns false.
    virtual bool updateInternal() = 0;
    virtual std::chrono::microseconds updateInterval() const { return std::chrono::seconds{ 2 }; }

    high_resolution_clock::time_point m_lastUpdateTime;
};

}
