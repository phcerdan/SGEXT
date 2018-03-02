#include "catch_header.h"
#include "spatial_histograms.hpp"

/**
 * Spatial Graph.
 *     o
 *     |
 *     o-o
 *     |
 *     o
 */
struct spatial_graph {
    using GraphType = SG::GraphAL;
    GraphType g;
    using vertex_iterator =
        typename boost::graph_traits<GraphType>::vertex_iterator;
    using edge_iterator =
        typename boost::graph_traits<GraphType>::edge_iterator;
    spatial_graph() {
        using boost::add_edge;
        this->g = GraphType(4);
        // Add edge with an associated SpatialEdge at construction.
        SG::PointType n3{{0, 3, 0}};
        SG::PointType n2{{0, 2, 0}};
        SG::PointType n1{{0, 1, 0}};
        SG::PointType p0{{0, 0, 0}};
        SG::PointType e1{{1, 0, 0}};
        SG::PointType e2{{2, 0, 0}};
        SG::PointType s1{{0, -1, 0}};
        SG::PointType s2{{0, -2, 0}};
        SG::PointType s3{{0, -3, 0}};

        g[0].pos = n3;
        g[1].pos = p0;
        g[2].pos = s3;
        g[3].pos = e2;

        SG::SpatialEdge se01;
        se01.edge_points.insert(std::end(se01.edge_points), {n1, n2});
        add_edge(0, 1, se01, g);
        SG::SpatialEdge se12;
        se12.edge_points.insert(std::end(se12.edge_points), {s1, s2});
        add_edge(1, 2, se12, g);
        SG::SpatialEdge se13;
        se13.edge_points.insert(std::end(se13.edge_points), {e1});
        add_edge(1, 3, se13, g);
    }
};

TEST_CASE_METHOD(spatial_graph,
                 "histogram degrees",
                 "[histograms][degrees]")
{
    auto degrees = SG::compute_degrees(g);
    auto histo_degrees = SG::histogram_degrees(degrees);
    std::cout << "Degrees Histogram" << std::endl;
    CHECK(histo_degrees.name == "degrees");
    std::cout << "Print counts with breaks" << std::endl;
    histo_degrees.PrintBreaksAndCounts(std::cout);
    std::cout << "Print counts with centers" << std::endl;
    histo_degrees.PrintCentersAndCounts(std::cout);
    std::cout << "Print centers" << std::endl;
    histo_degrees.PrintCenters(std::cout);
    std::cout << "Print counts" << std::endl;
    histo_degrees.PrintCounts(std::cout);
    std::cout << "Print breaks" << std::endl;
    histo_degrees.PrintBreaks(std::cout);
}

TEST_CASE_METHOD(spatial_graph,
                 "histogram distances",
                 "[histograms][distances]")
{
    auto distances = SG::compute_distances(g);
    auto histo_distances = SG::histogram_distances(distances);
    std::cout << "Distance Histogram" << std::endl;
    CHECK(histo_distances.name == "distances");
    histo_distances.PrintBreaksAndCounts(std::cout);
}

TEST_CASE_METHOD(spatial_graph,
                 "histogram angles",
                 "[histograms][angles][cosines]")
{
    auto angles = SG::compute_angles(g);
    auto histo_angles = SG::histogram_angles(angles);
    std::cout << "Angles Histogram" << std::endl;
    CHECK(histo_angles.name == "angles");
    histo_angles.PrintBreaksAndCounts(std::cout);

    // compute cosines from angles, not from g
    auto cosines = SG::compute_cosines(angles);
    auto histo_cosines = SG::histogram_cosines(cosines);
    std::cout << "Cosines Histogram" << std::endl;
    CHECK(histo_cosines.name == "cosines");
    histo_cosines.PrintBreaksAndCounts(std::cout);
}
