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

#include "array_utilities.hpp"
#include "gmock/gmock.h"
#include "shortest_path.hpp"
#include "spatial_graph.hpp"
#include "spatial_graph_utilities.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

struct ShortestPathFixture : public ::testing::Test {
  using GraphType = SG::GraphAL;
  using vertex_descriptor = GraphType::vertex_descriptor;
  GraphType g;

  void SetUp() override {
    using boost::add_edge;
    this->g = GraphType(3);

    // Add edge with an associated SpatialEdge at construction.
    SG::PointType p0{{0, 0, 0}};
    SG::PointType e1{{1, 0, 0}};
    SG::PointType e2{{2, 0, 0}};
    SG::PointType e05{{0.5, 0, 0}};
    SG::PointType e15{{1.5, 0, 0}};

    g[0].pos = p0;
    g[1].pos = e1;
    g[2].pos = e2;

    SG::SpatialEdge se05;
    se05.edge_points.insert(std::end(se05.edge_points), {e05});
    add_edge(0, 1, se05, g);
    SG::SpatialEdge se15;
    se15.edge_points.insert(std::end(se15.edge_points), {e15});
    add_edge(1, 2, se15, g);
  }
};

TEST_F(ShortestPathFixture, works) {
  // Setup data
  vertex_descriptor start_vertex = 0;
  vertex_descriptor end_vertex = 2;
  bool verbose = true;
  auto shortest_path =
      SG::compute_shortest_path(start_vertex, end_vertex, g, verbose);
  std::copy(shortest_path.begin(), shortest_path.end(),
      std::ostream_iterator<vertex_descriptor>(std::cout, ", "));
  std::cout << std::endl;
  // Read data
  EXPECT_EQ(shortest_path.size(), 3);
  std::vector<vertex_descriptor> expected_shortest_path = {0,1,2};
  EXPECT_EQ(shortest_path, expected_shortest_path);
}
