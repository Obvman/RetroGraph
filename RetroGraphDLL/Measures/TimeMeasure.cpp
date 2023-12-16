module Measures.TimeMeasure;

import "WindowsHeaderUnit.h";

namespace rg {

TimeMeasure::TimeMeasure() 
    : m_systemTime{ getCurrentLocalTime() }
    , m_uptime{ getCurrentUptime() } {

}

bool TimeMeasure::updateInternal() {
    bool timeChanged{ false };

    auto newSystemTime{ getCurrentLocalTime() };
    if (m_systemTime != newSystemTime) {
        m_systemTime = newSystemTime;
        timeChanged = true;
    }

    auto newUptime{ getCurrentUptime() };
    if (m_uptime != newUptime) {
        m_uptime = newUptime;
        timeChanged = true;
    }

    return timeChanged;
}

local_time<seconds> TimeMeasure::getCurrentLocalTime() const {
    return time_point_cast<seconds>(current_zone()->to_local(system_clock::now()));
}

seconds TimeMeasure::getCurrentUptime() const {
    return duration_cast<seconds>(milliseconds(GetTickCount64()));
}

} // namespace rg
