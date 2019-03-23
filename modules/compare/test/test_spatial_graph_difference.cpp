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

#include "gmock/gmock.h"
#include "FixtureSquareCrossGraph.hpp"
#include "spatial_graph_difference.hpp"

#include "graph_points_locator.hpp"
#include "get_vtk_points_from_graph.hpp"
#include "visualize_spatial_graph.hpp"

TEST_F(FixtureSquareCrossGraph, spatial_graph_difference_SquareCrossMinusCross) {
  bool verbose = true;
  if(verbose) std::cout << "Difference: SquareCross - Cross" << std::endl;
  double radius = 2.0;
  auto g_diff = spatial_graph_difference(g_square_cross, g_cross, radius, verbose);
  // expected after difference: g_square
  EXPECT_EQ(boost::num_vertices(g_diff), boost::num_vertices(g_square));
  EXPECT_EQ(boost::num_edges(g_diff), boost::num_edges(g_square));

  auto points_map_pair = SG::get_vtk_points_from_graph(g_diff);
  SG::visualize_spatial_graph_with_points(g_diff, points_map_pair.first, 0.8);
}
