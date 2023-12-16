export module UnitTests.Test_CallbackEvent;

import RG.Core;

import "Catch2HeaderUnit.h";

TEST_CASE("Core::CallbackEvent. Callback Event", "[callback_event]") {
    rg::CallbackEvent<> e;
    bool eventTriggered{ false };
    auto handle{ e.attach([&]() { eventTriggered = true; }) };

    SECTION("Simple raise") {
        e.raise();
        REQUIRE(eventTriggered);
    }

    SECTION("Detached raise") {
        e.detach(handle);
        e.raise();
        REQUIRE(!eventTriggered);
    }

    SECTION("Double detach") {
        e.detach(handle);
        REQUIRE_NOTHROW(e.detach(handle));
        REQUIRE_NOTHROW(e.raise());
        REQUIRE(!eventTriggered);
    }

    e.detach(handle);
}

TEST_CASE("Core::CallbackEvent. Boolean", "[callback_event]") {
    rg::CallbackEvent<bool> e;
    std::optional<bool> eventTriggered{ std::nullopt };
    auto handle{ e.attach([&](bool value) { eventTriggered = value; }) };

    SECTION("Nothing modified before raise") {
        REQUIRE(!eventTriggered.has_value());
    }

    SECTION("Raise with arg") {
        e.raise(false);
        REQUIRE(eventTriggered.has_value());
        REQUIRE(!*eventTriggered);
    }

    e.detach(handle);
}

TEST_CASE("Core::CallbackEvent. Multiple Clients", "[callback_event]") {
    rg::CallbackEvent<int> e;

    SECTION("Two clients") {
        int eventValue1{ 0 };
        int eventValue2{ 0 };
        auto handle1{ e.attach([&](int value) { eventValue1 = value; }) };
        auto handle2{ e.attach([&](int value) { eventValue2 = value + 1; }) };

        e.raise(1);
        REQUIRE(eventValue1 == 1);
        REQUIRE(eventValue2 == 2);

        e.detach(handle1);
        e.detach(handle2);
    }

    SECTION("Three clients") {
        int eventValue1{ 0 };
        int eventValue2{ 0 };
        int eventValue3{ 0 };
        auto handle1{ e.attach([&](int value) { eventValue1 = value; }) };
        auto handle2{ e.attach([&](int value) { eventValue2 = value + 1; }) };
        auto handle3{ e.attach([&](int value) { eventValue3 = value + 2; }) };

        e.raise(1);
        REQUIRE(eventValue1 == 1);
        REQUIRE(eventValue2 == 2);
        REQUIRE(eventValue3 == 3);

        e.detach(handle1);
        e.detach(handle2);
        e.detach(handle3);
    }
}
