#include "../catch2.hpp"

import Core.Strings;

using namespace rg;

TEST_CASE("Core::Strings. To integer conversions", "[str]") {
    REQUIRE(rg::strToNum<int>("0") == 0);
    REQUIRE(rg::strToNum<int>("10") == 10);
}
