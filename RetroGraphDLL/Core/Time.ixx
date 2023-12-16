export module RG.Core:Time;

import std.core;

namespace rg {

using namespace std::chrono;

export template<class Clock, class Duration>
void sleepUntil(const time_point<Clock, Duration>& timePoint) {
    std::this_thread::sleep_until(timePoint - 10us);
    while (timePoint >= Clock::now())
        std::this_thread::sleep_for(5us);
}

export template<class Clock, class Duration, class Result = milliseconds>
auto since(time_point<Clock, Duration> const& start) {
    return duration_cast<Result>(Clock::now() - start);
}

export class Timer {
public:
    Timer(microseconds interval)
        : m_startTime{ steady_clock::now() }
        , m_interval{ interval } {

    }

    void restart() { m_startTime = steady_clock::now(); }
    bool hasElapsed() const { return since(m_startTime) > m_interval; }

private:
    microseconds m_interval;
    steady_clock::time_point m_startTime;
};

}
