export module UnitTests.Test_CPUMeasure;

import RG.Measures;
import RG.Measures.DataSources;

import std.core;

import "Catch2HeaderUnit.h";

constexpr std::chrono::milliseconds testMeasureUpdateInterval{ 10 };

export class TestCPUDataSource : public rg::ICPUDataSource {
public:
    void update() override {}
    const std::string& getCPUName() const override { return cpuName; }
    float getCPUUsage() const override { return cpuUsage; }
    int getNumCores() const override { return numCores; };
    float getCoreUsage(int coreIdx) const override { return usages[coreIdx]; }
    float getClockSpeed() const override { return cpuClockSpeed; }
    float getVoltage() const override { return voltage; }
    float getTemp(int coreIdx) const override { return temps[coreIdx]; }

    std::string cpuName{};
    int numCores{ 0 };
    float cpuUsage{ 0.0f };
    float cpuClockSpeed{ 0.0f };
    float voltage{ 0.0f };
    std::vector<float> usages;
    std::vector<float> temps;
};

TEST_CASE("Measures::CPUMeasure. Update", "[measure]") {
    auto cpuDataSource{ std::make_unique<TestCPUDataSource>() };
    auto* cpuDataSourceRaw{ cpuDataSource.get() };
    rg::CPUMeasure measure{ testMeasureUpdateInterval, std::move(cpuDataSource) };

    cpuDataSourceRaw->cpuName = "Test CPU";
    cpuDataSourceRaw->numCores = 2;
    cpuDataSourceRaw->cpuUsage = 0.4f;
    cpuDataSourceRaw->cpuClockSpeed = 3.4f;
    cpuDataSourceRaw->voltage = 1.125f;
    cpuDataSourceRaw->usages = { 0.5f, 0.3f };
    cpuDataSourceRaw->temps = { 42.0f, 45.0f };

    float eventUsage = -1.0f;
    const auto handle{ measure.onCPUUsage.attach([&](float usage) { eventUsage = usage; }) };

    std::vector<float> eventCoreUsage{ -1.0f, -1.0f };
    const auto handle2{ measure.onCPUCoreUsage.attach([&](int i, float usage) { eventCoreUsage[i] = usage; }) };

    SECTION("Slow update changes data") {
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();

        REQUIRE(measure.getCPUName() == "Test CPU");
        REQUIRE(measure.getNumCores() == 2);
        REQUIRE(measure.getClockSpeed() == 3.4f);
        REQUIRE(measure.getVoltage() == 1.125f);
        REQUIRE(measure.getTemp(0) == 42.0f);
        REQUIRE(measure.getTemp(1) == 45.0f);

        REQUIRE(eventUsage == 0.4f);
        REQUIRE(eventCoreUsage[0] == 0.5f);
        REQUIRE(eventCoreUsage[1] == 0.3f);
    }

    SECTION("Multiple update calls") {
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);

        SECTION("Update when no change since last update") {
            measure.update();

            REQUIRE(measure.getCPUName() == "Test CPU");
            REQUIRE(measure.getNumCores() == 2);
            REQUIRE(measure.getClockSpeed() == 3.4f);
            REQUIRE(measure.getVoltage() == 1.125f);
            REQUIRE(measure.getTemp(0) == 42.0f);
            REQUIRE(measure.getTemp(1) == 45.0f);

            REQUIRE(eventUsage == 0.4f);
            REQUIRE(eventCoreUsage[0] == 0.5f);
            REQUIRE(eventCoreUsage[1] == 0.3f);
        }

        SECTION("Update when change since last update") {
            cpuDataSourceRaw->cpuUsage = 0.6f;
            cpuDataSourceRaw->cpuClockSpeed = 4.4f;
            cpuDataSourceRaw->voltage = 1.165f;
            cpuDataSourceRaw->usages = { 0.55f, 0.65f };
            cpuDataSourceRaw->temps = { 62.0f, 55.0f };
            measure.update();

            REQUIRE(measure.getCPUName() == "Test CPU");
            REQUIRE(measure.getNumCores() == 2);
            REQUIRE(measure.getClockSpeed() == 4.4f);
            REQUIRE(measure.getVoltage() == 1.165f);
            REQUIRE(measure.getTemp(0) == 62.0f);
            REQUIRE(measure.getTemp(1) == 55.0f);

            REQUIRE(eventUsage == 0.6f);
            REQUIRE(eventCoreUsage[0] == 0.55f);
            REQUIRE(eventCoreUsage[1] == 0.65f);
        }
    }

    measure.onCPUUsage.detach(handle);
    measure.onCPUCoreUsage.detach(handle2);
}
