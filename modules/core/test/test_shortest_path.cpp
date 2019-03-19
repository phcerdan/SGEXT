/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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

TEST_F(ShortestPathFixture, compute_shortest_path) {
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

TEST_F(ShortestPathFixture, create_edge_from_path) {
  // Setup data
  vertex_descriptor start_vertex = 0;
  vertex_descriptor end_vertex = 2;
  bool verbose = true;
  auto shortest_path =
      SG::compute_shortest_path(start_vertex, end_vertex, g, verbose);
  auto sg_edge = SG::create_edge_from_path(shortest_path, g);
  std::cout << "Edge From shortest path 0-2" << std::endl;
  std::cout << sg_edge << std::endl;
  std::vector<SG::PointType> expected_sg_edge_points =
  {{1.5, 0, 0}, {1,0,0}, {0.5,0,0}};
  EXPECT_EQ(sg_edge.edge_points, expected_sg_edge_points);
}
