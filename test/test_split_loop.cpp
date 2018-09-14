#include "catch_header.h"
#include "spatial_graph.hpp"
#include "split_loop.hpp"

TEST_CASE("split_loop", "[split_loop]") {
    std::cout << "Split loop" << std::endl;
    using SpatialGraph = SG::GraphType;
    using vertex_descriptor =
        typename boost::graph_traits<SpatialGraph>::vertex_descriptor;
    using SpatialEdge = typename boost::edge_bundle_type<SpatialGraph>::type;
    SG::PointType p0{{0, 0, 0}};
    SG::PointType n1{{0, 1, 0}};
    SG::PointType n2{{0, 2, 0}};
    SG::PointType e1n2{{1, 2, 0}};
    SG::PointType e2n2{{2, 2, 0}};
    SG::PointType e2n1{{2, 1, 0}};
    SG::PointType e2{{2, 0, 0}};
    SG::PointType e1{{1, 0, 0}};
    auto sg = SpatialGraph(1);
    vertex_descriptor vertex_id = 0;
    sg[vertex_id].pos = p0;
    SpatialEdge sg_edge;
    sg_edge.edge_points.insert(std::end(sg_edge.edge_points),
                               {n1, n2, e1n2, e2n2, e2n1, e2, e1});
    SG::split_loop(vertex_id, sg_edge, sg);
    CHECK(num_vertices(sg) == 2);
    CHECK(num_edges(sg) == 2);
    CHECK(sg[1].pos == e2n2);
    SpatialEdge::PointContainer expected1 = {{n1, n2, e1n2}};
    std::sort(expected1.begin(), expected1.end());
    SpatialEdge::PointContainer expected2 = {{e1, e2, e2n1}};
    std::sort(expected2.begin(), expected2.end());
    auto edges = boost::edges(sg);
    for (; edges.first != edges.second; ++edges.first) {
        auto &created_sg_edge = sg[*edges.first];
        auto &points = created_sg_edge.edge_points;
        std::sort(points.begin(), points.end());
        bool equal_edge_points = (points == expected1 || points == expected2);
        CHECK(equal_edge_points == true);
    }
}
