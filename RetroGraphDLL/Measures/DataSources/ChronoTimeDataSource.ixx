export module RG.Measures.DataSources:ChronoTimeDataSource;

import :ITimeDataSource;

namespace rg {

export class ChronoTimeDataSource : public ITimeDataSource {
public:
    TimeData getTimeData() const override;

private:
    local_time<seconds> getCurrentLocalTime() const;
    seconds getCurrentUptime() const;
};

}
