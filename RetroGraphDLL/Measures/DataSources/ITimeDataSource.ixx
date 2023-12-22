export module RG.Measures.DataSources:ITimeDataSource;

import std.core;

namespace rg {

using namespace std::chrono;

export struct TimeData {
    local_time<seconds> localTime{};
    seconds uptime{ 0 };

    auto operator<=>(const TimeData&) const = default;
};

export class ITimeDataSource {
public:
    virtual ~ITimeDataSource() = default;

    virtual TimeData getTimeData() const = 0;
};

} // namespace rg
