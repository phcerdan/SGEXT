#include "compute_graph_properties.hpp"
#include "gmock/gmock.h"

/**
 * Spatial Graph.
 *     o
 *     |
 *     o-o
 *     |
 *     o
 */
struct SpatialGraphFixture : public ::testing::Test {
    using GraphType = SG::GraphAL;
    GraphType g;
    void SetUp() override {
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

TEST_F(SpatialGraphFixture, compute_angles_cosines) {
    constexpr auto pi = 3.14159265358979323846;
    auto angles = SG::compute_angles(g);
    std::sort(angles.begin(), angles.end());
    std::cout << "Angles" << std::endl;
    std::ostream_iterator<double> out_it(std::cout, ", ");
    std::copy(angles.begin(), angles.end(), out_it);
    std::cout << std::endl;
    std::vector<double> expected_angles = {pi, pi / 2.0, pi / 2.0};
    std::sort(expected_angles.begin(), expected_angles.end());
    EXPECT_EQ(angles.size(), 3);
    EXPECT_EQ(angles, expected_angles);

    std::cout << "Cosines" << std::endl;
    // compute cosines from angles, not from g
    auto cosines = SG::compute_cosines(angles);
    std::sort(cosines.begin(), cosines.end());
    std::ostream_iterator<double> out_it_cos(std::cout, ", ");
    std::copy(cosines.begin(), cosines.end(), out_it_cos);
    std::cout << std::endl;
    std::vector<double> expected_cosines = {-1.0, 0.0, 0.0};
    std::sort(expected_cosines.begin(), expected_cosines.end());
    EXPECT_EQ(cosines.size(), 3);
    for (size_t i = 0; i < cosines.size(); i++) {
        EXPECT_NEAR(cosines[i], expected_cosines[i], 0.0000000000000001);
    }
}

struct PlusSymbolFixture : public SpatialGraphFixture {
    void SetUp() override {
        SpatialGraphFixture::SetUp();
        SG::PointType w1{{-1, 0, 0}};
        SG::PointType w2{{-2, 0, 0}};
        auto added = boost::add_vertex(this->g);
        this->g[added].pos = w2;

        SG::SpatialEdge se14;
        se14.edge_points.insert(std::end(se14.edge_points), {w1});
        boost::add_edge(1, added, se14, this->g);
    }
};

TEST_F(PlusSymbolFixture, compute_angles_cosines) {
    constexpr auto pi = 3.14159265358979323846;
    auto angles = SG::compute_angles(g);
    std::sort(angles.begin(), angles.end());
    std::cout << "Angles" << std::endl;
    std::ostream_iterator<double> out_it(std::cout, ", ");
    std::copy(angles.begin(), angles.end(), out_it);
    std::cout << std::endl;
    std::vector<double> expected_angles = {pi, pi / 2.0, pi / 2.0,
                                           pi, pi / 2.0, pi / 2.0};
    std::sort(expected_angles.begin(), expected_angles.end());
    EXPECT_EQ(angles.size(), 6);
    EXPECT_EQ(angles, expected_angles);

    std::cout << "Cosines" << std::endl;
    // compute cosines from angles, not from g
    auto cosines = SG::compute_cosines(angles);
    std::sort(cosines.begin(), cosines.end());
    std::ostream_iterator<double> out_it_cos(std::cout, ", ");
    std::copy(cosines.begin(), cosines.end(), out_it_cos);
    std::cout << std::endl;
    std::vector<double> expected_cosines = {-1.0, 0.0, 0.0, -1.0, 0.0, 0.0};
    std::sort(expected_cosines.begin(), expected_cosines.end());
    EXPECT_EQ(cosines.size(), 6);
    for (size_t i = 0; i < cosines.size(); i++) {
        EXPECT_NEAR(cosines[i], expected_cosines[i], 0.0000000000000001);
    }
}

struct OneEdge : public ::testing::Test {
    using GraphType = SG::GraphAL;
    GraphType g;
    void SetUp() override {
        this->g = GraphType(2);
        SG::PointType n3{{0, 3, 0}};
        this->g[0].pos = n3;
        this->g[1].pos = n3;
        boost::add_edge(0, 1, this->g);
    }
};

TEST_F(OneEdge, compute_angles_cosines) {
    auto angles = SG::compute_angles(g);
    EXPECT_EQ(angles.empty(), true);
}

struct TwoParallelEdgesFixture : public ::testing::Test {
    using GraphType = SG::GraphAL;
    GraphType g;
    void SetUp() override {
        this->g = GraphType(2);
        SG::PointType n3{{0, 3, 0}};
        this->g[0].pos = n3;
        this->g[1].pos = n3;
        boost::add_edge(0, 1, this->g);
        boost::add_edge(1, 0, this->g);
    }
};

TEST_F(TwoParallelEdgesFixture, compute_angles_cosines) {
    auto edges = boost::num_edges(this->g);
    std::cout << "Edges of two_paralell edges : " << edges << std::endl;
    EXPECT_EQ(edges, 2);
    bool no_ignore_parallel_edges = false;
    auto angles = SG::compute_angles(g, 0, no_ignore_parallel_edges);
    std::sort(angles.begin(), angles.end());
    std::cout << "Angles" << std::endl;
    std::ostream_iterator<double> out_it(std::cout, ", ");
    std::copy(angles.begin(), angles.end(), out_it);
    std::cout << std::endl;
    std::vector<double> expected_angles = {0.0, 0.0};
    std::sort(expected_angles.begin(), expected_angles.end());
    EXPECT_EQ(angles.size(), 2);
    for (size_t i = 0; i < angles.size(); i++) {
        EXPECT_NEAR(angles[i], expected_angles[i], 0.0000000000000001);
    }

    std::cout << "Cosines" << std::endl;
    // compute cosines from angles, not from g
    auto cosines = SG::compute_cosines(angles);
    std::sort(cosines.begin(), cosines.end());
    std::ostream_iterator<double> out_it_cos(std::cout, ", ");
    std::copy(cosines.begin(), cosines.end(), out_it_cos);
    std::cout << std::endl;
    std::vector<double> expected_cosines = {1.0, 1.0};
    std::sort(expected_cosines.begin(), expected_cosines.end());
    EXPECT_EQ(cosines.size(), 2);
    for (size_t i = 0; i < cosines.size(); i++) {
        EXPECT_NEAR(cosines[i], expected_cosines[i], 0.0000000000000001);
    }
}

TEST_F(TwoParallelEdgesFixture, compute_ignoring_parallel_edges) {
    auto edges = boost::num_edges(this->g);
    std::cout << "Edges of two_paralell edges ignoring parallel edges : "
              << edges << std::endl;
    EXPECT_EQ(edges, 2);
    bool ignore_parallel_edges = true;
    auto angles = SG::compute_angles(g, 0, ignore_parallel_edges);
    std::sort(angles.begin(), angles.end());
    std::cout << "Angles" << std::endl;
    std::ostream_iterator<double> out_it(std::cout, ", ");
    std::copy(angles.begin(), angles.end(), out_it);
    std::cout << std::endl;
    EXPECT_EQ(angles.empty(), true);

    std::cout << "Cosines" << std::endl;
    // compute cosines from angles, not from g
    auto cosines = SG::compute_cosines(angles);
    std::sort(cosines.begin(), cosines.end());
    std::ostream_iterator<double> out_it_cos(std::cout, ", ");
    std::copy(cosines.begin(), cosines.end(), out_it_cos);
    std::cout << std::endl;
    EXPECT_EQ(cosines.empty(), true);
}

TEST_F(SpatialGraphFixture, compute_filtered_distances) {
    EXPECT_EQ(boost::num_edges(g), 3);
    size_t count0 = 0;
    size_t count1 = 0;
    size_t count2 = 0;
    const auto edges = boost::edges(g);
    for (auto ei = edges.first; ei != edges.second; ++ei) {
        const auto &ep = g[*ei].edge_points;
        size_t npoints = ep.size();
        if (npoints == 0) {
            count0++;
        }
        if (npoints == 1) {
            count1++;
        }
        if (npoints == 2) {
            count2++;
        }
    }
    EXPECT_EQ(count0, 0);
    EXPECT_EQ(count1, 1);
    EXPECT_EQ(count2, 2);
    auto distances_unfiltered = SG::compute_ete_distances(g);
    size_t minimum_size_edges = 1;
    // Discards edges with 0 points (or less than 1)
    auto distances_filtered_1 =
            SG::compute_ete_distances(g, minimum_size_edges);
    // Discards edges with 1 points (or less than 2)
    auto distances_filtered_2 = SG::compute_ete_distances(g, 2);
    auto distances_filtered_3 = SG::compute_ete_distances(g, 3);
    EXPECT_EQ(distances_filtered_3.empty(), true);
    EXPECT_EQ(distances_filtered_2.size(), 2);
    std::cout << "distances_filtered_2:" << std::endl;
    std::ostream_iterator<double> out_it_2(std::cout, ", ");
    std::copy(distances_filtered_2.begin(), distances_filtered_2.end(),
              out_it_2);
    std::cout << std::endl;
    EXPECT_EQ(distances_filtered_1.size(), 3); // No empty ep
    EXPECT_EQ(distances_unfiltered.size(), 3);
}

TEST_F(SpatialGraphFixture, compute_filtered_angles) {
    EXPECT_EQ(boost::num_edges(g), 3);
    size_t count0 = 0;
    size_t count1 = 0;
    size_t count2 = 0;
    const auto edges = boost::edges(g);
    for (auto ei = edges.first; ei != edges.second; ++ei) {
        const auto &ep = g[*ei].edge_points;
        size_t npoints = ep.size();
        if (npoints == 0) {
            count0++;
        }
        if (npoints == 1) {
            count1++;
        }
        if (npoints == 2) {
            count2++;
        }
    }
    EXPECT_EQ(count0, 0);
    EXPECT_EQ(count1, 1);
    EXPECT_EQ(count2, 2);
    auto angles_unfiltered = SG::compute_angles(g);
    size_t minimum_size_edges = 1;
    // Discards edges with 0 points (or less than 1)
    auto angles_filtered_1 = SG::compute_angles(g, minimum_size_edges);
    // Discards edges with 1 points (or less than 2)
    auto angles_filtered_2 = SG::compute_angles(g, 2);
    bool ignore_parallel_edges = true;
    auto angles_filtered_2_ignore =
            SG::compute_angles(g, 2, ignore_parallel_edges);
    auto angles_filtered_3 = SG::compute_angles(g, 3);
    EXPECT_EQ(angles_filtered_3.empty(), true);
    EXPECT_EQ(angles_filtered_2.size(), 1);
    std::cout << "angles_filtered_2:" << std::endl;
    std::ostream_iterator<double> out_it_2(std::cout, ", ");
    std::copy(angles_filtered_2.begin(), angles_filtered_2.end(), out_it_2);
    std::cout << std::endl;
    EXPECT_EQ(angles_filtered_2_ignore.size(), 1);
    EXPECT_EQ(angles_filtered_1.size(), 3); // No empty ep
    EXPECT_EQ(angles_unfiltered.size(), 3);
}

TEST_F(SpatialGraphFixture, filtered_distances_ignoring_end_nodes) {
    EXPECT_EQ(boost::num_edges(g), 3);
    size_t count0 = 0;
    size_t count1 = 0;
    size_t count2 = 0;
    const auto edges = boost::edges(g);
    for (auto ei = edges.first; ei != edges.second; ++ei) {
        const auto &ep = g[*ei].edge_points;
        size_t npoints = ep.size();
        if (npoints == 0) {
            count0++;
        }
        if (npoints == 1) {
            count1++;
        }
        if (npoints == 2) {
            count2++;
        }
    }
    EXPECT_EQ(count0, 0);
    EXPECT_EQ(count1, 1);
    EXPECT_EQ(count2, 2);
    auto distances_unfiltered = SG::compute_ete_distances(g);
    size_t minimum_size_edges = 0;
    bool ignore_end_nodes = true;
    auto distances_filtered_ignore_end_nodes =
            SG::compute_ete_distances(g, minimum_size_edges, ignore_end_nodes);
    EXPECT_EQ(distances_filtered_ignore_end_nodes.empty(), true);
    EXPECT_EQ(distances_unfiltered.size(), 3);
}

TEST_F(SpatialGraphFixture, filtered_angles_ignoring_end_nodes) {
    EXPECT_EQ(boost::num_edges(g), 3);
    size_t count0 = 0;
    size_t count1 = 0;
    size_t count2 = 0;
    const auto edges = boost::edges(g);
    for (auto ei = edges.first; ei != edges.second; ++ei) {
        const auto &ep = g[*ei].edge_points;
        size_t npoints = ep.size();
        if (npoints == 0) {
            count0++;
        }
        if (npoints == 1) {
            count1++;
        }
        if (npoints == 2) {
            count2++;
        }
    }
    EXPECT_EQ(count0, 0);
    EXPECT_EQ(count1, 1);
    EXPECT_EQ(count2, 2);
    auto angles_unfiltered = SG::compute_angles(g);
    size_t minimum_size_edges = 0;
    bool ignore_parallel_edges = false;
    bool ignore_end_nodes = true;
    auto angles_filtered_ignore_end_nodes = SG::compute_angles(
            g, minimum_size_edges, ignore_parallel_edges, ignore_end_nodes);
    EXPECT_EQ(angles_filtered_ignore_end_nodes.empty(), true); // No empty ep
    EXPECT_EQ(angles_unfiltered.size(), 3);
}
