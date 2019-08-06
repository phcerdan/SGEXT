/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "FixtureCloseGraphs.hpp"
#include "extend_low_info_graph.hpp"
#include "gmock/gmock.h"

#include "get_vtk_points_from_graph.hpp"
#include "graph_points_locator.hpp"
#include "visualize_spatial_graph.hpp"

TEST_F(FixtureCloseGraphs, works) {
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(moved_g0));
    graphs.push_back(std::cref(moved_g1));

    bool verbose = true;

    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs);
    auto &mergePoints = merger_map_pair.first;
    auto &idMap = merger_map_pair.second;
    auto octree = SG::build_octree_locator(merger_map_pair.first->GetPoints());
    double radius = 10.0;
    auto extended_g = extend_low_info_graph_via_dfs(graphs, idMap, octree,
                                                    radius, verbose);
    EXPECT_EQ(boost::num_vertices(extended_g), boost::num_vertices(moved_g0));

    auto points_map_pair = SG::get_vtk_points_from_graph(extended_g);
    SG::visualize_spatial_graph_with_points(extended_g, points_map_pair.first,
                                            0.8);
}
