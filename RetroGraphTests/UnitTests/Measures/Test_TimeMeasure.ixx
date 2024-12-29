export module UnitTests.Test_TimeMeasure;

import RG.Measures;
import RG.Measures.DataSources;

import std;

import "Catch2HeaderUnit.h";

using namespace std::chrono;

constexpr milliseconds testMeasureUpdateInterval{ 10 };

export class TestTimeDataSource : public rg::ITimeDataSource {
public:
    rg::TimeData getTimeData() const override { return m_timeData; }

    void setTimeData(const rg::TimeData& timeData) { m_timeData = timeData; }

    rg::TimeData m_timeData{};
};

TEST_CASE("Measures::TimeMeasure. Update", "[measure]") {
    constexpr local_time<seconds> initialLocalTime{ 10s };
    constexpr seconds initialUptime{ 10s };
    const rg::TimeData defaultTimeData{};
    rg::TimeData testTimeData{
        .localTime{ initialLocalTime },
        .uptime{ initialUptime },
    };

    auto timeDataSource{ std::make_unique<TestTimeDataSource>() };
    auto* timeDataSourceRaw{ timeDataSource.get() };
    rg::TimeMeasure measure{ testMeasureUpdateInterval, std::move(timeDataSource) };

    timeDataSourceRaw->setTimeData(testTimeData);

    SECTION("Instant update does not change data") {
        measure.update();
        REQUIRE(measure.getLocalTime() == defaultTimeData.localTime);
        REQUIRE(measure.getUptime() == defaultTimeData.uptime);
    }

    SECTION("Slow update changes data") {
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();
        REQUIRE(measure.getLocalTime() == initialLocalTime);
        REQUIRE(measure.getUptime() == initialUptime);
    }

    SECTION("Multiple update calls") {
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();

        SECTION("Update when no change since last update") {
            std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
            measure.update();
            REQUIRE(measure.getLocalTime() == initialLocalTime);
            REQUIRE(measure.getUptime() == initialUptime);
        }

        SECTION("Update when change since last update") {
            std::this_thread::sleep_for(testMeasureUpdateInterval * 2);

            testTimeData.localTime += 1s;
            testTimeData.uptime += 10s;
            timeDataSourceRaw->setTimeData(testTimeData);
            measure.update();
            REQUIRE(measure.getLocalTime() == initialLocalTime + 1s);
            REQUIRE(measure.getUptime() == initialUptime + 10s);
        }
    }
}
