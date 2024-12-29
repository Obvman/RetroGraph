export module RG.Measures:Measure;

import RG.Core;

import std;

namespace rg {

using namespace std::chrono;

export using PostUpdateEvent = CallbackEvent<>;

export class Measure {
public:
    Measure(std::optional<milliseconds> updateInterval)
        : m_lastUpdateTime{ steady_clock::now() }
        , m_updateInterval{ updateInterval } {}

    virtual ~Measure() = default;

    Measure(const Measure&) = delete;
    Measure& operator=(const Measure&) = delete;
    Measure(Measure&&) = delete;
    Measure& operator=(Measure&&) = delete;

    void update() {
        if (m_updateInterval) {
            if (since(m_lastUpdateTime) > *m_updateInterval) {
                if (updateInternal()) {
                    postUpdate.raise();
                }
                m_lastUpdateTime = high_resolution_clock::now();
            }
        }
    }

    void setUpdateInterval(std::optional<milliseconds> updateInterval) {
        m_updateInterval = updateInterval;
        m_lastUpdateTime = high_resolution_clock::now();
    }

    PostUpdateEvent postUpdate;

protected:
    // The implementation of measure updates. Returns true if any data was modified, otherwise returns false.
    virtual bool updateInternal() = 0;

    high_resolution_clock::time_point m_lastUpdateTime;
    std::optional<milliseconds> m_updateInterval;
};

} // namespace rg
