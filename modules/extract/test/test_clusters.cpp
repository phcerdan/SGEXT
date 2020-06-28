/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "detect_clusters.hpp"
#include "detect_clusters_visitor.hpp"
#include "spatial_graph.hpp"
#include "spatial_graph_utilities.hpp"

#include "gmock/gmock.h"

struct SpatialGraphBaseFixture {
    using GraphType = SG::GraphAL;
    GraphType g;
    using vertex_iterator =
            typename boost::graph_traits<GraphType>::vertex_iterator;
    using edge_iterator =
            typename boost::graph_traits<GraphType>::edge_iterator;
};

/**
 * Spatial Graph
 *     o
 *    /
 * o-o    <--- cluster 0
 * | |
 * | |
 * | |
 * o-o    <--- cluster 1
 *
 */
struct sg_clusters : public SpatialGraphBaseFixture, ::testing::Test {
    void SetUp() override {
        using boost::add_edge;
        this->g = GraphType(5);
        g[0].pos = {{0, 0, 0}};
        g[1].pos = {{1, 0, 0}};
        g[2].pos = {{1, 5, 0}};
        g[3].pos = {{0, 5, 0}};
        g[4].pos = {{2, 8, 0}};
        add_edge(0, 1, g);
        add_edge(1, 2, g);
        add_edge(2, 3, g);
        add_edge(3, 0, g);
        add_edge(2, 4, g);
    }
};

TEST_F(sg_clusters, detect_clusters_with_radius) {
    const double cluster_radius = 2.0;
    const bool use_centroids = false;
    const bool verbose = true;
    auto cluster_label_map = SG::detect_clusters_with_radius(
            g, cluster_radius, use_centroids,
            verbose);

    const auto number_of_nodes_belonging_to_any_cluster =
            cluster_label_map.size();
    EXPECT_EQ(number_of_nodes_belonging_to_any_cluster, 4);
    std::cout << "cluster_label_map:" << std::endl;
    for (auto &t : cluster_label_map)
        std::cout << "  " << t.first << " " << t.second << "\n";
    EXPECT_EQ(cluster_label_map[0], 0);
    EXPECT_EQ(cluster_label_map[1], 0);
    EXPECT_EQ(cluster_label_map[2], 2);
    EXPECT_EQ(cluster_label_map[3], 2);
}

TEST_F(sg_clusters, assign_cluster_label_to_spatial_node_id) {
    const double cluster_radius = 2.0;
    const bool use_centroids = false;
    auto cluster_label_map = SG::detect_clusters_with_radius(
            g, cluster_radius, use_centroids);
    SG::assign_cluster_label_to_spatial_node_id(g, cluster_label_map);
    EXPECT_EQ(g[0].id, 0);
    EXPECT_EQ(g[1].id, 0);
    EXPECT_EQ(g[2].id, 2);
    EXPECT_EQ(g[3].id, 2);
}
}
