#include "catch2.hpp"

import Core.Math;

TEST_CASE("Linear Interpolation", "[lerp]") {
    REQUIRE(rg::lerp(0.0f, 1.0f, 0.0f) == Approx{ 0.0f });
    REQUIRE(rg::lerp(0.0f, 1.0f, 1.0f) == Approx{ 1.0f });
    REQUIRE(rg::lerp(0.0f, 1.0f, 0.5f) == Approx{ 0.5f });
    REQUIRE(rg::lerp(-1.0f, 1.0f, 0.5f) == Approx{ 0.0f });
    REQUIRE(rg::lerp(-1.0f, 1.0f, 0.0f) == Approx{ -1.0f });
    REQUIRE(rg::lerp(-1.0f, 1.0f, 1.0f) == Approx{ 1.0f });
}
