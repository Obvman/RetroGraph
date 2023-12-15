#include "catch2.hpp"

import Core.Math;

int testSomething() { return 5; }

TEST_CASE("Linear Interpolation", "[lerp]") {

    REQUIRE(rg::lerp(0.0f, 1.0f, 0.0f) == 0.1f);
}
