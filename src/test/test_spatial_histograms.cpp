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
struct test_spatial_graph {
    using GraphType = SG::GraphAL;
    GraphType g;
    using vertex_iterator =
        typename boost::graph_traits<GraphType>::vertex_iterator;
    using edge_iterator =
        typename boost::graph_traits<GraphType>::edge_iterator;
    test_spatial_graph() {
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


TEST_CASE_METHOD(test_spatial_graph,
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

TEST_CASE_METHOD(test_spatial_graph,
                 "histogram degrees with bins",
                 "[histograms][degrees][bins]")
{
    auto degrees = SG::compute_degrees(g);
    size_t bins = 10;
    auto histo_degrees = SG::histogram_degrees(degrees, bins);
    std::cout << "Degrees Histogram with bins = " << bins << std::endl;
    CHECK(histo_degrees.name == "degrees");
    CHECK(histo_degrees.bins == bins);
    histo_degrees.PrintBreaksAndCounts(std::cout);
}

TEST_CASE_METHOD(test_spatial_graph,
                 "histogram distances",
                 "[histograms][distances]")
{
    auto distances = SG::compute_distances(g);
    auto histo_distances = SG::histogram_distances(distances);
    std::cout << "Distance Histogram" << std::endl;
    CHECK(histo_distances.name == "distances");
    histo_distances.PrintBreaksAndCounts(std::cout);
}

TEST_CASE_METHOD(test_spatial_graph,
                 "histogram distances with bins",
                 "[histograms][distances][bins]")
{
    auto distances = SG::compute_distances(g);
    size_t bins = 10;
    auto histo_distances = SG::histogram_distances(distances, bins);
    std::cout << "Distance Histogram with bins" << std::endl;
    CHECK(histo_distances.name == "distances");
    CHECK(histo_distances.bins == bins);
    histo_distances.PrintBreaksAndCounts(std::cout);
}

TEST_CASE_METHOD(test_spatial_graph,
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

TEST_CASE_METHOD(test_spatial_graph,
                 "histogram angles with bins",
                 "[histograms][angles][cosines][bins]")
{
    auto angles = SG::compute_angles(g);
    size_t bins = 10;
    auto histo_angles = SG::histogram_angles(angles, bins);
    std::cout << "Angles Histogram with bins = " << bins << std::endl;
    CHECK(histo_angles.name == "angles");
    CHECK(histo_angles.bins == bins);
    histo_angles.PrintBreaksAndCounts(std::cout);

    // compute cosines from angles, not from g
    auto cosines = SG::compute_cosines(angles);
    auto histo_cosines = SG::histogram_cosines(cosines, bins);
    std::cout << "Cosines Histogram with bins = " << bins << std::endl;
    CHECK(histo_cosines.name == "cosines");
    CHECK(histo_cosines.bins == bins);
    histo_cosines.PrintBreaksAndCounts(std::cout);
}

TEST_CASE_METHOD(test_spatial_graph,
                 "compute angles and cosines",
                 "[angles][angles][cosines]")
{
    constexpr auto pi = 3.14159265358979323846;
    auto angles = SG::compute_angles(g);
    std::sort(angles.begin(), angles.end());
    std::cout << "Angles" << std::endl;
    std::ostream_iterator<double> out_it (std::cout,", ");
    std::copy ( angles.begin(), angles.end(), out_it );
    std::cout <<  std::endl;
    std::vector<double> expected_angles = {pi, pi/2.0, pi/2.0};
    std::sort(expected_angles.begin(), expected_angles.end());
    CHECK(angles.size() == 3);
    CHECK(angles == expected_angles);

    std::cout << "Cosines" << std::endl;
    // compute cosines from angles, not from g
    auto cosines = SG::compute_cosines(angles);
    std::sort(cosines.begin(), cosines.end());
    std::ostream_iterator<double> out_it_cos (std::cout,", ");
    std::copy ( cosines.begin(), cosines.end(), out_it_cos );
    std::cout <<  std::endl;
    std::vector<double> expected_cosines = {-1.0, 0.0, 0.0};
    std::sort(expected_cosines.begin(), expected_cosines.end());
    CHECK(cosines.size() == 3);
    CHECK(cosines == expected_cosines);
}

struct edges_plus_symbol : public test_spatial_graph {
    edges_plus_symbol() : test_spatial_graph() {
        SG::PointType w1{{-1, 0, 0}};
        SG::PointType w2{{-2, 0, 0}};
        auto added = boost::add_vertex(this->g);
        this->g[added].pos = w2;

        SG::SpatialEdge se14;
        se14.edge_points.insert(std::end(se14.edge_points), {w1});
        boost::add_edge(1, added, se14, this->g);
    }
};

TEST_CASE_METHOD(edges_plus_symbol, "compute angles and cosines in plus", "[angles][cosines]") {
    constexpr auto pi = 3.14159265358979323846;
    auto angles = SG::compute_angles(g);
    std::sort(angles.begin(), angles.end());
    std::cout << "Angles" << std::endl;
    std::ostream_iterator<double> out_it (std::cout,", ");
    std::copy ( angles.begin(), angles.end(), out_it );
    std::cout <<  std::endl;
    std::vector<double> expected_angles = {
        pi, pi/2.0, pi/2.0,
        pi, pi/2.0, pi/2.0
    };
    std::sort(expected_angles.begin(), expected_angles.end());
    CHECK(angles.size() == 6);
    CHECK(angles == expected_angles);

    std::cout << "Cosines" << std::endl;
    // compute cosines from angles, not from g
    auto cosines = SG::compute_cosines(angles);
    std::sort(cosines.begin(), cosines.end());
    std::ostream_iterator<double> out_it_cos (std::cout,", ");
    std::copy ( cosines.begin(), cosines.end(), out_it_cos );
    std::cout <<  std::endl;
    std::vector<double> expected_cosines = {
        -1.0, 0.0, 0.0,
        -1.0, 0.0, 0.0
    };
    std::sort(expected_cosines.begin(), expected_cosines.end());
    CHECK(cosines.size() == 6);
    CHECK(cosines == expected_cosines);
}
