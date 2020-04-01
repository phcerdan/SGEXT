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

#include "FixtureCloseGraphs.hpp"
#include "extend_low_info_graph.hpp"
#include "gmock/gmock.h"

#include "get_vtk_points_from_graph.hpp"
#include "graph_points_locator.hpp"
#ifdef VISUALIZE
#include "visualize_spatial_graph.hpp"
#endif

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
#ifdef VISUALIZE
    SG::visualize_spatial_graph_with_points(extended_g, points_map_pair.first,
                                            0.8);
#endif
}
