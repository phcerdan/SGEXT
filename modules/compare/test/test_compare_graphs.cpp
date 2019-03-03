/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gmock/gmock.h"
#include "compare_graphs.hpp"
#include "filter_spatial_graph.hpp"
#include "FixtureMatchingGraphs.hpp"
#include "FixtureCloseGraphs.hpp"
#include "get_vtk_points_from_graph.hpp"
#include "graph_points_locator.hpp"
#include "spatial_graph_utilities.hpp"

// #include "visualize_spatial_graph.hpp"
// TEST_F(FixtureMatchingGraphs, visualize_it)
// {
//     SG::visualize_spatial_graph(g0);
//     SG::visualize_spatial_graph(g1);
//     SG::visualize_spatial_graph(gR);
// }

TEST_F(FixtureMatchingGraphs, works)
{

    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs);
    auto & mergePoints = merger_map_pair.first;
    auto & idMap = merger_map_pair.second;
    auto kdtree = SG::build_kdtree_locator(mergePoints->GetPoints());
    // g0 and g1 should have 13 unique points when combined
    EXPECT_EQ(kdtree->GetDataSet()->GetNumberOfPoints(), 13);
}


TEST_F(FixtureMatchingGraphs, compare_graphs)
{
    double radius = 0.6;
    // double radius = 1.0; // fails
    auto edges_nodes_to_remove = SG::remove_edges_and_nodes_from_high_info_graph(g0, g1, radius);
    const auto & removed_edges = edges_nodes_to_remove.first;
    const auto & removed_nodes = edges_nodes_to_remove.second;

    EXPECT_EQ(removed_edges.size(), 1);
    std::cout << "Removed edges: " << std::endl;
    for(const auto & edge : removed_edges){
        std::cout << edge << std::endl;
        EXPECT_TRUE(edge.m_source == 4 || edge.m_target == 4);
        EXPECT_TRUE(edge.m_source == 5 || edge.m_target == 5);
    }

    auto filtered_graph = SG::filter_by_sets(removed_edges, removed_nodes, g1);
    // auto filtered_graph= SG::compare_low_and_high_info_graphs(g0, g1, radius);
    EXPECT_EQ(boost::num_vertices(g1), 6);
    EXPECT_EQ(boost::num_vertices(filtered_graph), boost::num_vertices(g1));
    EXPECT_EQ(boost::num_edges(filtered_graph), boost::num_edges(g1) - 1);
};

TEST_F(FixtureCloseGraphs, works)
{
    // FixtureCloseGraphs applies a small shift to all positions of g1
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    EXPECT_EQ(boost::num_vertices(g0), 4);
    EXPECT_EQ(SG::num_edge_points(g0), 5);
    EXPECT_EQ(boost::num_vertices(g1), 6);
    EXPECT_EQ(SG::num_edge_points(g1), 7);
    EXPECT_EQ(boost::num_vertices(moved_g0), 4);
    EXPECT_EQ(SG::num_edge_points(moved_g0), 5);
    EXPECT_EQ(boost::num_vertices(moved_g1), 6);
    EXPECT_EQ(SG::num_edge_points(moved_g1), 7);
    {
        auto points_map_pair = get_vtk_points_from_graph(g0);
        EXPECT_EQ(points_map_pair.first->GetNumberOfPoints(), 9);
    }
    {
        auto points_map_pair = get_vtk_points_from_graph(moved_g1);
        EXPECT_EQ(points_map_pair.first->GetNumberOfPoints(), 13);
    }
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(moved_g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs);
    auto & mergePoints = merger_map_pair.first;
    auto & idMap = merger_map_pair.second;
    EXPECT_EQ(mergePoints->GetPoints()->GetNumberOfPoints(), 22);
    auto kdtree = SG::build_kdtree_locator(mergePoints->GetPoints());
    // g0 and moved_g1 should have 22 (9 + 13) unique points when combined
    EXPECT_EQ(kdtree->GetDataSet()->GetNumberOfPoints(), 22);
}

TEST_F(FixtureCloseGraphs, compare_graphs)
{
    auto & low_info_g = g0;
    auto & high_info_g = moved_g1;
    auto edges_nodes_to_remove = SG::remove_edges_and_nodes_from_high_info_graph(low_info_g, high_info_g);
    const auto & removed_edges = edges_nodes_to_remove.first;
    const auto & removed_nodes = edges_nodes_to_remove.second;

    EXPECT_EQ(removed_edges.size(), 1);
    std::cout << "Removed edges: " << std::endl;
    for(const auto & edge : removed_edges){
        std::cout << edge << std::endl;
        EXPECT_TRUE(edge.m_source == 4 || edge.m_target == 4);
        EXPECT_TRUE(edge.m_source == 5 || edge.m_target == 5);
    }

    auto filtered_graph = SG::filter_by_sets(removed_edges, removed_nodes, high_info_g);
    // auto filtered_graph= SG::compare_low_and_high_info_graphs(low_info_g, high_info_g);
    EXPECT_EQ(boost::num_vertices(high_info_g), 6);
    EXPECT_EQ(boost::num_vertices(filtered_graph), boost::num_vertices(high_info_g));
    EXPECT_EQ(boost::num_edges(filtered_graph), boost::num_edges(high_info_g) - 1);
};
