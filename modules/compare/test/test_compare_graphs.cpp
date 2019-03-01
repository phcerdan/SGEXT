/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gmock/gmock.h"
#include "compare_graphs.hpp"
#include "filter_spatial_graph.hpp"
#include "graph_locator_fixtures.hpp"
#include "get_vtk_points_from_graph.hpp"
#include "graph_points_locator.hpp"

// #include "visualize_spatial_graph.hpp"
// TEST_F(MatchingGraphsFixture, visualize_it)
// {
//     SG::visualize_spatial_graph(g0);
//     SG::visualize_spatial_graph(g1);
//     SG::visualize_spatial_graph(gR);
// }

TEST_F(MatchingGraphsFixture, works)
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


TEST_F(MatchingGraphsFixture, compare_graphs)
{
    auto edges_nodes_to_remove = SG::remove_edges_and_nodes_from_high_info_graph(g0, g1);
    const auto & removed_edges = edges_nodes_to_remove.first;
    const auto & removed_nodes = edges_nodes_to_remove.second;

    EXPECT_EQ(removed_edges.size(), 1);
    std::cout << "Removed edges: " << std::endl;
    for(const auto & edge : removed_edges){
        std::cout << edge << std::endl;
        EXPECT_TRUE(edge.m_source == 4 || edge.m_target == 4);
        EXPECT_TRUE(edge.m_source == 5 || edge.m_target == 5);
    }

    // auto filtered_graph = SG::filter_by_sets(removed_edges, removed_nodes, g1);
    auto filtered_graph= SG::compare_low_and_high_info_graphs(g0, g1);
    EXPECT_EQ(boost::num_vertices(g1), 6);
    EXPECT_EQ(boost::num_vertices(filtered_graph), boost::num_vertices(g1));
    EXPECT_EQ(boost::num_edges(filtered_graph), boost::num_edges(g1) - 1);
};
