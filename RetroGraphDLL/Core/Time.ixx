export module Core.Time;

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

}
