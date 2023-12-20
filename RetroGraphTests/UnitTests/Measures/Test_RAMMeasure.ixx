export module UnitTests.Test_RAMMeasure;

import RG.Measures;
import RG.Measures.DataSources;

import std.core;

import "Catch2HeaderUnit.h";

using namespace std::chrono;

constexpr milliseconds testMeasureUpdateInterval{ 10 };

export class TestRAMDataSource : public rg::IRAMDataSource {
public:
    float getRAMUsage() const { return usage; }
    uint64_t getRAMCapacity() const { return 0U; }

    float usage{ 0.0f };
};

TEST_CASE("Measures::RAMMeasure. Update", "[measure]") {
    constexpr float initialUsage{ 0.2f };

    auto ramDataSource{ std::make_unique<TestRAMDataSource>() };
    auto* ramDataSourceRaw{ ramDataSource.get()};
    rg::RAMMeasure measure{ testMeasureUpdateInterval, std::move(ramDataSource) };

    ramDataSourceRaw->usage = initialUsage;

    float eventUsage = -1.0f;
    const auto handle{ measure.onRAMUsage.attach([&](float usage) { eventUsage = usage; }) };

    SECTION("Instant update does not raise usage event") {
        measure.update();
        REQUIRE(eventUsage == -1.0f);
    }

    SECTION("Slow update raises usage event") {
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();
        REQUIRE(eventUsage == initialUsage);
    }

    SECTION("Multiple update calls") {
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();

        SECTION("Update when no change since last update") {
            std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
            measure.update();
            REQUIRE(eventUsage == initialUsage);
        }

        SECTION("Update when change since last update") {
            std::this_thread::sleep_for(testMeasureUpdateInterval * 2);

            ramDataSourceRaw->usage = 0.5f;
            measure.update();
            REQUIRE(eventUsage == 0.5f);
        }
    }

    measure.onRAMUsage.detach(handle);
}
