export module RG.Measures:TimeMeasure;

import :Measure;

import std.core;

namespace rg {

using namespace std::chrono;

export class TimeMeasure : public Measure {
public:
    TimeMeasure();
    ~TimeMeasure() noexcept = default;

    local_time<seconds> getLocalTime() const { return m_systemTime; }
    seconds getUptime() const { return m_uptime; }

protected:
    /* Updates each drive with new values */
    bool updateInternal() override;

private:
    local_time<seconds> getCurrentLocalTime() const;
    seconds getCurrentUptime() const;

    local_time<seconds> m_systemTime;
    seconds m_uptime;
};

} // namespace rg
