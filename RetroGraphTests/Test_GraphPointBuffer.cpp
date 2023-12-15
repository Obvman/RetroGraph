#include "catch2.hpp"

import Widgets.Graph.GraphPointBuffer;

namespace rgunit {

TEST_CASE("Graph Point Buffer", "[graph_point_buffer]") {
    rg::GraphPointBuffer pb{ 2 };
    REQUIRE(pb.head() == 0);
    REQUIRE(pb.tail() == 0);

}

}
