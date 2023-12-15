#include "../catch2.hpp"

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

    SECTION("operator[]") {
        pb.pushPoint(1.0f);
        pb.pushPoint(2.0f);

        REQUIRE(pb[0].y == Approx{ 2.0f });
    }
}

TEST_CASE("Widgets::Graph::GraphPointBuffer. 3 Elements", "[graph_point_buffer]") {
    constexpr size_t numPoints{ 3 };
    constexpr float defaultY{ -1.0f };

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

    SECTION("operator[]") {
        pb.pushPoint(1.0f);

        REQUIRE(pb[0].y == Approx{ defaultY });
        REQUIRE(pb[1].y == Approx{ defaultY });
        REQUIRE(pb[2].y == Approx{ 1.0f });
    }

    SECTION("operator[]") {
        pb.pushPoint(1.0f);
        pb.pushPoint(2.0f);
        pb.pushPoint(3.0f);

        REQUIRE(pb[0].y == Approx{ 1.0f });
        REQUIRE(pb[1].y == Approx{ 2.0f });
        REQUIRE(pb[2].y == Approx{ 3.0f });

        pb.pushPoint(4.0f);

        REQUIRE(pb[0].y == Approx{ 2.0f });
        REQUIRE(pb[1].y == Approx{ 3.0f });
        REQUIRE(pb[2].y == Approx{ 4.0f });
    }
}

TEST_CASE("Widgets::Graph::GraphPointBuffer. Setting Points", "[graph_point_buffer]") {
    constexpr size_t numPoints{ 3 };

    rg::GraphPointBuffer pb{ numPoints };

    SECTION("Same size") {
        pb.setPoints(std::vector<float>{ 0.0f, 0.5f, 1.0f });

        REQUIRE(pb.numPoints() == numPoints);
        REQUIRE(pb.head() == 2);
        REQUIRE(pb.tail() == 0);
        REQUIRE(pb[0].y == Approx{ 0.0f });
        REQUIRE(pb[1].y == Approx{ 0.5f });
        REQUIRE(pb[2].y == Approx{ 1.0f });
    }

    SECTION("Fewer points") {
        pb.setPoints(std::vector<float>{ 0.0f, 0.5f });

        REQUIRE(pb.numPoints() == 2);
        REQUIRE(pb.head() == 1);
        REQUIRE(pb.tail() == 0);
        REQUIRE(pb[0].y == Approx{ 0.0f });
        REQUIRE(pb[1].y == Approx{ 0.5f });
    }

    SECTION("More points") {
        pb.setPoints(std::vector<float>{ 0.0f, 0.5f, 1.0f, 1.5f, 2.0f, 2.5f });

        REQUIRE(pb.numPoints() == 6);
        REQUIRE(pb.head() == 5);
        REQUIRE(pb.tail() == 0);
        REQUIRE(pb[0].y == Approx{ 0.0f });
        REQUIRE(pb[1].y == Approx{ 0.5f });
        REQUIRE(pb[2].y == Approx{ 1.0f });
        REQUIRE(pb[3].y == Approx{ 1.5f });
        REQUIRE(pb[4].y == Approx{ 2.0f });
        REQUIRE(pb[5].y == Approx{ 2.5f });
    }

    SECTION("Down to one point") {
        pb.setPoints(std::vector<float>{ 0.0f });

        REQUIRE(pb.numPoints() == 1);
        REQUIRE(pb.head() == 0);
        REQUIRE(pb.tail() == 0);
        REQUIRE(pb[0].y == Approx{ 0.0f });
    }

    SECTION("Empty out") {
        pb.setPoints(std::vector<float>{});

        REQUIRE(pb.numPoints() == 0);
        REQUIRE(pb.head() == 0);
        REQUIRE(pb.tail() == 0);
    }

    SECTION("Overwrites pushed points") {
        pb.pushPoint(1.0f);
        pb.pushPoint(2.0f);
        pb.pushPoint(3.0f);
        pb.setPoints(std::vector<float>{ -5.0f, -6.0f, -7.0f });

        REQUIRE(pb[0].y == Approx{ -5.0f });
        REQUIRE(pb[1].y == Approx{ -6.0f });
        REQUIRE(pb[2].y == Approx{ -7.0f });
    }
}
