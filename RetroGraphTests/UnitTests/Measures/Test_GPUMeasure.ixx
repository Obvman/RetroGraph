export module UnitTests.Test_GPUMeasure;

import RG.Measures;
import RG.Measures.DataSources;

import std;

import "Catch2HeaderUnit.h";

constexpr std::chrono::milliseconds testMeasureUpdateInterval{ 10 };

export class TestGPUDataSource : public rg::IGPUDataSource {
public:
    const std::string& getGPUName() const override { return gpuName; }
    const std::string& getDriverVersion() const override { return driverVersion; }
    float getGPUUsage() const override { return gpuUsage; }
    int getGPUTemp() const override { return gpuTemp; }
    int getGPUTotalMemoryKB() const override { return totalMemoryKB; }
    int getGPUAvailableMemoryKB() const override { return availableMemoryKB; }
    int getGPUFrameBufferSizeKB() const override { return gpuFrameBufferSizeKB; }
    int getGPUCoreCount() const override { return gpuCoreCount; }

    std::string gpuName;
    std::string driverVersion;
    float gpuUsage;
    int gpuTemp;
    int totalMemoryKB;
    int availableMemoryKB;
    int gpuCoreCount;
    int gpuFrameBufferSizeKB;
};

TEST_CASE("Measures::GPUMeasure. Update", "[measure]") {
    auto gpuDataSource{ std::make_unique<TestGPUDataSource>() };
    auto* gpuDataSourceRaw{ gpuDataSource.get() };
    rg::GPUMeasure measure{ testMeasureUpdateInterval, std::move(gpuDataSource) };

    gpuDataSourceRaw->gpuName = "Test GPU";
    gpuDataSourceRaw->driverVersion = "1.1.2";
    gpuDataSourceRaw->gpuUsage = 0.4f;
    gpuDataSourceRaw->gpuTemp = 80;
    gpuDataSourceRaw->totalMemoryKB = 4000000;
    gpuDataSourceRaw->availableMemoryKB = 2000000;
    gpuDataSourceRaw->gpuCoreCount = 2000;
    gpuDataSourceRaw->gpuFrameBufferSizeKB = 100000;

    float eventUsage = -1.0f;
    const auto handle{ measure.onGPUUsage.attach([&](float usage) { eventUsage = usage; }) };

    SECTION("Slow update changes data") {
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();

        REQUIRE(measure.getGPUName() == "Test GPU");
        REQUIRE(measure.getDriverVersion() == "1.1.2");
        REQUIRE(measure.getGPUTemp() == 80);
        REQUIRE(measure.getGPUTotalMemoryKB() == 4000000);
        REQUIRE(measure.getGPUAvailableMemoryKB() == 2000000);
        REQUIRE(measure.getGPUFrameBufferSizeKB() == 100000);
        REQUIRE(measure.getGPUCoreCount() == 2000);

        REQUIRE(eventUsage == 0.4f);
    }

    SECTION("Multiple update calls") {
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);

        SECTION("Update when no change since last update") {
            measure.update();

            REQUIRE(measure.getGPUName() == "Test GPU");
            REQUIRE(measure.getDriverVersion() == "1.1.2");
            REQUIRE(measure.getGPUTemp() == 80);
            REQUIRE(measure.getGPUTotalMemoryKB() == 4000000);
            REQUIRE(measure.getGPUAvailableMemoryKB() == 2000000);
            REQUIRE(measure.getGPUFrameBufferSizeKB() == 100000);
            REQUIRE(measure.getGPUCoreCount() == 2000);

            REQUIRE(eventUsage == 0.4f);
        }

        SECTION("Update when change since last update") {
            gpuDataSourceRaw->gpuUsage = 0.6f;
            gpuDataSourceRaw->gpuTemp = 89;
            gpuDataSourceRaw->availableMemoryKB = 1000000;
            measure.update();

            REQUIRE(measure.getGPUName() == "Test GPU");
            REQUIRE(measure.getDriverVersion() == "1.1.2");
            REQUIRE(measure.getGPUTemp() == 89);
            REQUIRE(measure.getGPUTotalMemoryKB() == 4000000);
            REQUIRE(measure.getGPUAvailableMemoryKB() == 1000000);
            REQUIRE(measure.getGPUFrameBufferSizeKB() == 100000);
            REQUIRE(measure.getGPUCoreCount() == 2000);

            REQUIRE(eventUsage == 0.6f);
        }
    }

    measure.onGPUUsage.detach(handle);
}
