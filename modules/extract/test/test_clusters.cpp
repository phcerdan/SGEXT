/* ********************************************************************
 * Copyright (C) 2020 Pablo Hernandez-Cerdan.
 *
 * This file is part of SGEXT: http://github.com/phcerdan/sgext.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * *******************************************************************/

#include "collapse_clusters.hpp"
#include "collapse_clusters_visitor.hpp"
#include "detect_clusters.hpp"
#include "detect_clusters_visitor.hpp"
#include "spatial_graph.hpp"
#include "spatial_graph_utilities.hpp"
#include "merge_nodes.hpp" // for remove_parallel_edges

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
 * o-o    <--- cluster label: 2
 * | |
 * | |
 * | |
 * o-o    <--- cluster label: 0
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
    for (const auto &t : cluster_label_map) {
        std::cout << "  " << t.first << " " << t.second << "\n";
    }
    EXPECT_EQ(cluster_label_map[0], 0);
    EXPECT_EQ(cluster_label_map[1], 0);
    EXPECT_EQ(cluster_label_map[2], 2);
    EXPECT_EQ(cluster_label_map[3], 2);
}

TEST_F(sg_clusters, assign_label_to_spatial_node_id) {
    const double cluster_radius = 2.0;
    const bool use_centroids = false;
    auto cluster_label_map = SG::detect_clusters_with_radius(
            g, cluster_radius, use_centroids);
    SG::assign_label_to_spatial_node_id(g, cluster_label_map);
    EXPECT_EQ(g[0].id, 0);
    EXPECT_EQ(g[1].id, 0);
    EXPECT_EQ(g[2].id, 2);
    EXPECT_EQ(g[3].id, 2);
}

TEST_F(sg_clusters, collapse_clusters) {
    const double cluster_radius = 2.0;
    const bool use_centroids = true;
    const bool verbose = true;
    auto cluster_label_map = SG::detect_clusters_with_radius(
            g, cluster_radius, use_centroids,
            verbose);
    auto collapsed_graph = SG::collapse_clusters(g, cluster_label_map, verbose);
    EXPECT_EQ( boost::num_vertices(collapsed_graph), 3);
    // There is one parallel edge between clusters:
    //    o
    //   /
    //  o    cluster label: 1
    // / \
    // \ /
    //  o    cluster label: 0
    EXPECT_EQ( boost::num_edges(collapsed_graph), 3);
    SG::print_degrees(collapsed_graph);
    SG::print_edges(collapsed_graph);
    EXPECT_EQ(collapsed_graph[0].pos, g[0].pos);
    EXPECT_EQ(collapsed_graph[1].pos, g[2].pos);
    EXPECT_EQ(collapsed_graph[2].pos, g[4].pos);
}

TEST_F(sg_clusters, collapse_clusters_and_remove_larger_parallel_edges) {
    const double cluster_radius = 2.0;
    const bool use_centroids = true;
    const bool verbose = true;
    auto cluster_label_map = SG::detect_clusters_with_radius(
            g, cluster_radius, use_centroids,
            verbose);
    auto collapsed_graph = SG::collapse_clusters(g, cluster_label_map, verbose);
    auto collapsed_graph_with_no_parallel_edges =
        SG::remove_parallel_edges(collapsed_graph);
    EXPECT_EQ( boost::num_vertices(collapsed_graph_with_no_parallel_edges), 3);
    // There is one parallel edge between clusters:
    //    o
    //   /
    //  o    cluster label: 1
    //   \
    //   /
    //  o    cluster label: 0
    EXPECT_EQ( boost::num_edges(collapsed_graph_with_no_parallel_edges), 2);
    SG::print_degrees(collapsed_graph_with_no_parallel_edges);
    SG::print_edges(collapsed_graph_with_no_parallel_edges);
    // EXPECT_EQ(collapsed_graph_with_no_parallel_edges[0].pos, g[0].pos);
    // EXPECT_EQ(collapsed_graph_with_no_parallel_edges[1].pos, g[2].pos);
    // EXPECT_EQ(collapsed_graph_with_no_parallel_edges[2].pos, g[4].pos);
}

TEST_F(sg_clusters, collapse_specific_clusters) {
    const double cluster_radius = 2.0;
    const bool use_centroids = true;
    const bool verbose = true;
    auto cluster_label_map = SG::detect_clusters_with_radius(
            g, cluster_radius, use_centroids,
            verbose);
    // allowed cluster_labels: {0 and/or 2}
    std::vector<GraphType::vertex_descriptor> cluster_labels = {0};
    const auto trimmed_cluster_label_map = SG::trim_vertex_to_single_label_map(
            cluster_labels, cluster_label_map);
    std::cout << "trimmed_cluster_label_map:" << std::endl;
    for (const auto &t : trimmed_cluster_label_map) {
        std::cout << "  " << t.first << " " << t.second << "\n";
    }
    EXPECT_EQ(trimmed_cluster_label_map.at(0), 0);
    EXPECT_EQ(trimmed_cluster_label_map.at(1), 0);

    auto collapsed_graph = SG::collapse_clusters(g, trimmed_cluster_label_map, verbose);
    // auto collapsed_graph = SG::collapse_specific_clusters(cluster_labels, g, cluster_label_map, verbose);
    EXPECT_EQ( boost::num_vertices(collapsed_graph), 4);
    EXPECT_EQ( boost::num_edges(collapsed_graph), 4);
    SG::print_degrees(collapsed_graph);
    SG::print_edges(collapsed_graph);
    EXPECT_EQ( boost::degree(0, collapsed_graph), 2);
    EXPECT_EQ( boost::degree(1, collapsed_graph), 3);
    EXPECT_EQ( boost::degree(2, collapsed_graph), 2);
    EXPECT_EQ( boost::degree(3, collapsed_graph), 1);
}

