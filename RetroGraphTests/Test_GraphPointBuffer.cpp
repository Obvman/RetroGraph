#include "catch2.hpp"

import Widgets.Graph.GraphPointBuffer;

TEST_CASE("Widgets::Graph::GraphPointBuffer. Empty", "[graph_point_buffer]") {
    rg::GraphPointBuffer pb{ 0 };
    REQUIRE(pb.numPoints() == 0);
    REQUIRE(pb.bufferSize() == 0);
    REQUIRE(pb.head() == 0);
    REQUIRE(pb.tail() == 0);
}

TEST_CASE("Widgets::Graph::GraphPointBuffer. 1 Element", "[graph_point_buffer]") {
    constexpr size_t numPoints{ 1 };
    rg::GraphPointBuffer pb{ numPoints };
    REQUIRE(pb.numPoints() == numPoints);
    REQUIRE(pb.bufferSize() >= numPoints);
    REQUIRE(pb.head() == 0);
    REQUIRE(pb.tail() == 0);

    SECTION("Pushing single point") {
        pb.pushPoint(1.0f);

        REQUIRE(pb.numPoints() == numPoints);
        REQUIRE(pb.back() == pb.front());
        REQUIRE(pb.back()->y == Approx{ 1.0f });
    }

    SECTION("Pushing two points") {
        bool didBufferReallocate{ pb.pushPoint(1.0f) };
        REQUIRE(didBufferReallocate);
        didBufferReallocate = pb.pushPoint(2.0f);
        REQUIRE(didBufferReallocate);

        REQUIRE(pb.numPoints() == numPoints);
        REQUIRE(pb.back() == pb.front());
        REQUIRE(pb.back()->y == Approx{ 2.0f });
    }
}

TEST_CASE("Widgets::Graph::GraphPointBuffer. 3 Elements", "[graph_point_buffer]") {
    constexpr size_t numPoints{ 3 };
    rg::GraphPointBuffer pb{ numPoints };
    REQUIRE(pb.numPoints() == numPoints);
    REQUIRE(pb.bufferSize() >= numPoints);
    REQUIRE(pb.head() == 2);
    REQUIRE(pb.tail() == 0);

    SECTION("Pushing single point") {
        pb.pushPoint(1.0f);

        REQUIRE(pb.back()->y == Approx{ 1.0f });
    }

    SECTION("Pushing two points") {
        bool didBufferReallocate{ pb.pushPoint(1.0f) };
        REQUIRE(!didBufferReallocate);
        didBufferReallocate = pb.pushPoint(2.0f);
        REQUIRE(!didBufferReallocate);

        REQUIRE(pb.back()->y == Approx{ 2.0f });
    }

    SECTION("Pushing three points") {
        pb.pushPoint(1.0f);
        pb.pushPoint(2.0f);
        pb.pushPoint(3.0f);

        REQUIRE(pb.back()->y == Approx{ 3.0f });
    }
}
