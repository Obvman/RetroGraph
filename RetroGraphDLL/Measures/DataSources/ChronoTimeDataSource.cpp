module RG.Measures.DataSources:ChronoTimeDataSource;

import "WindowsHeaderUnit.h";

namespace rg {

TimeData ChronoTimeDataSource::getTimeData() const {
    return TimeData{ .localTime{ getCurrentLocalTime() }, .uptime{ getCurrentUptime() } };
}

local_time<seconds> ChronoTimeDataSource::getCurrentLocalTime() const {
    return time_point_cast<seconds>(current_zone()->to_local(system_clock::now()));
}

seconds ChronoTimeDataSource::getCurrentUptime() const {
    return duration_cast<seconds>(milliseconds(GetTickCount64()));
}

}
