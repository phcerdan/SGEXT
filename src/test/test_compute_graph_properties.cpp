#include "catch_header.h"
#include "compute_graph_properties.hpp"

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
    for (size_t i = 0; i < cosines.size(); i++)
        CHECK( cosines[i] == Approx(expected_cosines[i]).margin(0.0000000000000001) );
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

TEST_CASE_METHOD(edges_plus_symbol,
        "compute angles and cosines in plus",
        "[angles][cosines]")
{
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
    for (size_t i = 0; i < cosines.size(); i++)
        CHECK( cosines[i] == Approx(expected_cosines[i]).margin(0.0000000000000001) );
}

struct test_one_edge {
    using GraphType = SG::GraphAL;
    GraphType g;
    test_one_edge() {
        this->g = GraphType(2);
        SG::PointType n3{{0, 3, 0}};
        SG::PointType s3{{0, -3, 0}};
        this->g[0].pos = n3;
        this->g[1].pos = n3;
        boost::add_edge(0,1,this->g);
    }
};

TEST_CASE_METHOD(test_one_edge,
        "compute angles and cosines in one edge",
        "[angles][cosines]") {
    constexpr auto pi = 3.14159265358979323846;
    auto angles = SG::compute_angles(g);
    CHECK(angles.empty() == true);
}

struct test_two_parallel_edges {
    using GraphType = SG::GraphAL;
    GraphType g;
    test_two_parallel_edges() {
        this->g = GraphType(2);
        SG::PointType n3{{0, 3, 0}};
        SG::PointType s3{{0, -3, 0}};
        this->g[0].pos = n3;
        this->g[1].pos = n3;
        boost::add_edge(0,1,this->g);
        boost::add_edge(1,0,this->g);
    }
};

TEST_CASE_METHOD(test_two_parallel_edges,
        "compute angles and cosines in two_parallel_edges",
        "[angles][cosines]") {
    auto edges = boost::num_edges(this->g);
    std::cout << "Edges of two_paralell edges : " << edges << std::endl;
    CHECK(edges == 2);
    bool no_ignore_parallel_edges = false;
    constexpr auto pi = 3.14159265358979323846;
    auto angles = SG::compute_angles(g, 0, no_ignore_parallel_edges);
    std::sort(angles.begin(), angles.end());
    std::cout << "Angles" << std::endl;
    std::ostream_iterator<double> out_it (std::cout,", ");
    std::copy ( angles.begin(), angles.end(), out_it );
    std::cout <<  std::endl;
    std::vector<double> expected_angles = { 0.0, 0.0 };
    std::sort(expected_angles.begin(), expected_angles.end());
    CHECK(angles.size() == 2);
    for (size_t i = 0; i < angles.size(); i++)
        CHECK( angles[i] == Approx(expected_angles[i]).margin(0.0000000000000001) );

    std::cout << "Cosines" << std::endl;
    // compute cosines from angles, not from g
    auto cosines = SG::compute_cosines(angles);
    std::sort(cosines.begin(), cosines.end());
    std::ostream_iterator<double> out_it_cos (std::cout,", ");
    std::copy ( cosines.begin(), cosines.end(), out_it_cos );
    std::cout <<  std::endl;
    std::vector<double> expected_cosines = { 1.0, 1.0 };
    std::sort(expected_cosines.begin(), expected_cosines.end());
    CHECK(cosines.size() == 2);
    for (size_t i = 0; i < cosines.size(); i++)
        CHECK( cosines[i] == Approx(expected_cosines[i]).margin(0.0000000000000001) );
}

TEST_CASE_METHOD(test_two_parallel_edges,
        "compute angles and cosines in two_parallel_edges ignoring parallel edges",
        "[angles][cosines]") {
    auto edges = boost::num_edges(this->g);
    std::cout << "Edges of two_paralell edges ignoring parallel edges : " << edges << std::endl;
    CHECK(edges == 2);
    bool ignore_parallel_edges = true;
    auto angles = SG::compute_angles(g, 0, ignore_parallel_edges);
    std::sort(angles.begin(), angles.end());
    std::cout << "Angles" << std::endl;
    std::ostream_iterator<double> out_it (std::cout,", ");
    std::copy ( angles.begin(), angles.end(), out_it );
    std::cout <<  std::endl;
    CHECK(angles.empty() == true);

    std::cout << "Cosines" << std::endl;
    // compute cosines from angles, not from g
    auto cosines = SG::compute_cosines(angles);
    std::sort(cosines.begin(), cosines.end());
    std::ostream_iterator<double> out_it_cos (std::cout,", ");
    std::copy ( cosines.begin(), cosines.end(), out_it_cos );
    std::cout <<  std::endl;
    CHECK(cosines.empty() == true);
}
TEST_CASE_METHOD(test_spatial_graph,
                 "compute filtered distances",
                 "[filtered][distances]")
{
    CHECK(boost::num_edges(g) == 3);
    size_t count0 = 0;
    size_t count1 = 0;
    size_t count2 = 0;
    const auto edges = boost::edges(g);
    for (auto ei = edges.first; ei != edges.second; ++ei) {
        const auto & ep = g[*ei].edge_points;
        size_t npoints = ep.size();
        if(npoints == 0)
            count0++;
        if(npoints == 1)
            count1++;
        if(npoints == 2)
            count2++;
    }
    CHECK(count0 == 0);
    CHECK(count1 == 1);
    CHECK(count2 == 2);
    auto distances_unfiltered = SG::compute_ete_distances(g);
    size_t minimum_size_edges = 1;
    // Discards edges with 0 points (or less than 1)
    auto distances_filtered_1 = SG::compute_ete_distances(g, minimum_size_edges);
    // Discards edges with 1 points (or less than 2)
    auto distances_filtered_2 = SG::compute_ete_distances(g, 2);
    auto distances_filtered_3 = SG::compute_ete_distances(g, 3);
    CHECK(distances_filtered_3.empty() == true);
    CHECK(distances_filtered_2.size() == 2);
    std::cout << "distances_filtered_2:" << std::endl;
    std::ostream_iterator<double> out_it_2 (std::cout,", ");
    std::copy ( distances_filtered_2.begin(), distances_filtered_2.end(), out_it_2 );
    std::cout << std::endl;
    CHECK(distances_filtered_1.size() == 3); // No empty ep
    CHECK(distances_unfiltered.size() == 3);
}

TEST_CASE_METHOD(test_spatial_graph,
                 "compute filtered angles",
                 "[filtered][angles]")
{
    CHECK(boost::num_edges(g) == 3);
    size_t count0 = 0;
    size_t count1 = 0;
    size_t count2 = 0;
    const auto edges = boost::edges(g);
    for (auto ei = edges.first; ei != edges.second; ++ei) {
        const auto & ep = g[*ei].edge_points;
        size_t npoints = ep.size();
        if(npoints == 0)
            count0++;
        if(npoints == 1)
            count1++;
        if(npoints == 2)
            count2++;
    }
    CHECK(count0 == 0);
    CHECK(count1 == 1);
    CHECK(count2 == 2);
    auto angles_unfiltered = SG::compute_angles(g);
    size_t minimum_size_edges = 1;
    // Discards edges with 0 points (or less than 1)
    auto angles_filtered_1 = SG::compute_angles(g, minimum_size_edges);
    // Discards edges with 1 points (or less than 2)
    auto angles_filtered_2 = SG::compute_angles(g, 2);
    bool ignore_parallel_edges = true;
    auto angles_filtered_2_ignore = SG::compute_angles(g, 2, ignore_parallel_edges );
    auto angles_filtered_3 = SG::compute_angles(g, 3);
    CHECK(angles_filtered_3.empty() == true);
    CHECK(angles_filtered_2.size() == 1);
    std::cout << "angles_filtered_2:" << std::endl;
    std::ostream_iterator<double> out_it_2 (std::cout,", ");
    std::copy ( angles_filtered_2.begin(), angles_filtered_2.end(), out_it_2 );
    std::cout << std::endl;
    CHECK(angles_filtered_2_ignore.size() == 1);
    CHECK(angles_filtered_1.size() == 3); // No empty ep
    CHECK(angles_unfiltered.size() == 3);
}
