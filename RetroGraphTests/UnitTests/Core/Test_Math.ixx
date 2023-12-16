export module UnitTests.Test_Math;

import Core.Math;

import "Catch2HeaderUnit.h";

// #TODO more unit tests:
// viewport utilities
// Measures - 
//   Format of data (usage measures return float 0.0f -> 1.0f, time strings)

TEST_CASE("Core::Math. Linear Interpolation", "[lerp]") {
    REQUIRE(rg::lerp(0.0f, 1.0f, 0.0f) == Approx{ 0.0f });
    REQUIRE(rg::lerp(0.0f, 1.0f, 1.0f) == Approx{ 1.0f });
    REQUIRE(rg::lerp(0.0f, 1.0f, 0.5f) == Approx{ 0.5f });
    REQUIRE(rg::lerp(-1.0f, 1.0f, 0.5f) == Approx{ 0.0f });
    REQUIRE(rg::lerp(-1.0f, 1.0f, 0.0f) == Approx{ -1.0f });
    REQUIRE(rg::lerp(-1.0f, 1.0f, 1.0f) == Approx{ 1.0f });
}
