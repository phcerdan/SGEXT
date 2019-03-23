/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
