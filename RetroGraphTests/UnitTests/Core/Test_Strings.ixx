export module UnitTests.Test_Strings;

import RG.Core;

import "Catch2HeaderUnit.h";

using namespace rg;

TEST_CASE("Core::Strings. To integer conversions", "[str]") {
    REQUIRE(rg::strToNum<int>("0") == 0);
    REQUIRE(rg::strToNum<int>("10") == 10);
    REQUIRE(rg::strToNum<int>("-10") == -10);

    REQUIRE_THROWS(rg::strToNum<int>("100000000000"));
}

TEST_CASE("Core::Strings. To unsigned integer conversions", "[str]") {
    REQUIRE(rg::strToNum<unsigned int>("0") == 0);
    REQUIRE(rg::strToNum<unsigned int>("10") == 10);

    REQUIRE_THROWS(rg::strToNum<unsigned int>("-10"));
}
