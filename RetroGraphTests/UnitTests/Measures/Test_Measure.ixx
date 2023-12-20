export module UnitTests.Test_Measure;

import RG.Measures;

import "Catch2HeaderUnit.h";

using namespace std::chrono;

constexpr milliseconds testMeasureUpdateInterval{ 10 };

class TestMeasure : public rg::Measure {
public:
    TestMeasure() : Measure{ testMeasureUpdateInterval } {}

protected:
    bool updateInternal() override { return true; }
};

class StaticTestMeasure : public rg::Measure {
public:
    StaticTestMeasure() : Measure{ std::nullopt } {}

protected:
    bool updateInternal() override { return false; }
};

TEST_CASE("Measures::Measure. Update", "[measure]") {
    TestMeasure measure;
    bool updateEventTriggered{ false };
    auto handle{ measure.postUpdate.attach([&]() { updateEventTriggered = true; }) };

    SECTION("Instant update does not trigger event") {
        measure.update();
        REQUIRE(!updateEventTriggered);
    }

    SECTION("Slow update triggers event") {
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();
        REQUIRE(updateEventTriggered);
    }

    SECTION("Multiple update calls") {
        measure.update();
        REQUIRE(!updateEventTriggered);

        updateEventTriggered = false;
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();
        REQUIRE(updateEventTriggered);

        updateEventTriggered = false;
        measure.update();
        REQUIRE(!updateEventTriggered);

        updateEventTriggered = false;
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();
        REQUIRE(updateEventTriggered);
    }

    measure.postUpdate.detach(handle);
}

TEST_CASE("Measures::Measure. Static Measure Update", "[measure]") {
    StaticTestMeasure measure;
    bool updateEventTriggered{ false };
    auto handle{ measure.postUpdate.attach([&]() {updateEventTriggered = true; }) };

    SECTION("Instant update does not trigger event") {
        measure.update();
        REQUIRE(!updateEventTriggered);
    }

    SECTION("Slow update does not trigger event") {
        std::this_thread::sleep_for(testMeasureUpdateInterval * 2);
        measure.update();
        REQUIRE(!updateEventTriggered);
    }

    measure.postUpdate.detach(handle);
}
