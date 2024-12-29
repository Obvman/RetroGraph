export module RG.Measures:TimeMeasure;

import :Measure;

import RG.Measures.DataSources;

import std;

namespace rg {

using namespace std::chrono;

export class TimeMeasure : public Measure {
public:
    TimeMeasure(std::chrono::milliseconds updateInterval, std::unique_ptr<const ITimeDataSource> timeDataSource);
    ~TimeMeasure() noexcept = default;

    local_time<seconds> getLocalTime() const { return m_timeData.localTime; }
    seconds getUptime() const { return m_timeData.uptime; }

protected:
    /* Updates each drive with new values */
    bool updateInternal() override;

private:
    std::unique_ptr<const ITimeDataSource> m_timeDataSource;
    TimeData m_timeData;
};

} // namespace rg
