module RG.Measures:TimeMeasure;

namespace rg {

TimeMeasure::TimeMeasure(std::chrono::milliseconds updateInterval,
                         std::unique_ptr<const ITimeDataSource> timeDataSource)
    : Measure{ updateInterval }
    , m_timeDataSource{ std::move(timeDataSource) }
    , m_timeData{ m_timeDataSource->getTimeData() } {

}

bool TimeMeasure::updateInternal() {
    const TimeData oldTimeData{ m_timeData };
    m_timeData = m_timeDataSource->getTimeData();
    return oldTimeData != m_timeData;
}

} // namespace rg
