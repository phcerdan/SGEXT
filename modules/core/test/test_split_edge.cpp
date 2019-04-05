/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gmock/gmock.h"
#include "split_edge.hpp"
#include "spatial_graph_utilities.hpp"

struct SplitEdgeFixture : public ::testing::Test {
  using GraphType = SG::GraphAL;
  using vertex_descriptor = GraphType::vertex_descriptor;
  GraphType g_one_edge_point;
  GraphType g_two_edge_point;
  GraphType g_reverse_two_edge_point;
  SG::PointType p0{{0, 0, 0}};
  SG::PointType e1{{1, 0, 0}};
  SG::PointType e2{{2, 0, 0}};
  SG::PointType e05{{0.5, 0, 0}};
  SG::PointType e15{{1.5, 0, 0}};

  void CreateOneEdgePointGraph() {
    using boost::add_edge;
    this->g_one_edge_point = GraphType(2);

    g_one_edge_point[0].pos = p0;
    g_one_edge_point[1].pos = e1;

    SG::SpatialEdge se;
    se.edge_points.insert(std::end(se.edge_points), {e05});
    add_edge(0, 1, se, g_one_edge_point);
  }

  void CreateTwoEdgePointGraph() {
    using boost::add_edge;
    this->g_two_edge_point = GraphType(2);

    g_two_edge_point[0].pos = p0;
    g_two_edge_point[1].pos = e2;

    SG::SpatialEdge se;
    se.edge_points.insert(std::end(se.edge_points), {e05, e15});
    add_edge(0, 1, se, g_two_edge_point);
  }
  void CreateReverseTwoEdgePointGraph() {
    using boost::add_edge;
    this->g_reverse_two_edge_point = GraphType(2);

    g_reverse_two_edge_point[0].pos = p0;
    g_reverse_two_edge_point[1].pos = e2;

    SG::SpatialEdge se;
    se.edge_points.insert(std::end(se.edge_points), {e15, e05});
    add_edge(0, 1, se, g_reverse_two_edge_point);
  }

  void SetUp() override {
    CreateOneEdgePointGraph();
    CreateTwoEdgePointGraph();
    CreateReverseTwoEdgePointGraph();
  }
};

TEST_F(SplitEdgeFixture, OneEdgePointWorks) {
    std::cout << "BEFORE" << std::endl;
    SG::print_spatial_edges(g_one_edge_point);
    EXPECT_EQ(SG::num_edge_points(g_one_edge_point), 1);
    auto edge = boost::edge(0, 1, g_one_edge_point);
    auto ed = edge.first;
    SG::SplitEdge<GraphType> splitEdge =
        SG::split_edge(e05, ed, g_one_edge_point);
    EXPECT_TRUE(splitEdge.point_exist_in_edge);
    EXPECT_EQ(splitEdge.edge_point_index, 0);
    std::cout << "AFTER" << std::endl;
    SG::print_spatial_edges(g_one_edge_point);
    EXPECT_EQ(boost::num_vertices(g_one_edge_point), 3);
    EXPECT_EQ(boost::num_edges(g_one_edge_point), 2);
    auto num_ep = SG::num_edge_points(g_one_edge_point);
    EXPECT_EQ(num_ep, 0);
}

TEST_F(SplitEdgeFixture, TwoEdgePointWorks) {
    std::cout << "BEFORE" << std::endl;
    SG::print_spatial_edges(g_two_edge_point);
    EXPECT_EQ(SG::num_edge_points(g_two_edge_point), 2);
    auto edge = boost::edge(0, 1, g_two_edge_point);
    auto ed = edge.first;
    SG::SplitEdge<GraphType> splitEdge =
        SG::split_edge(e15, ed, g_two_edge_point);
    EXPECT_TRUE(splitEdge.point_exist_in_edge);
    EXPECT_EQ(splitEdge.edge_point_index, 1);
    std::cout << "AFTER" << std::endl;
    SG::print_spatial_edges(g_two_edge_point);
    EXPECT_EQ(boost::num_vertices(g_two_edge_point), 3);
    EXPECT_EQ(boost::num_edges(g_two_edge_point), 2);
    auto num_ep = SG::num_edge_points(g_two_edge_point);
    EXPECT_EQ(num_ep, 1);
}

TEST_F(SplitEdgeFixture, ReverseTwoEdgePointWorks) {
    std::cout << "BEFORE" << std::endl;
    SG::print_spatial_edges(g_reverse_two_edge_point);
    EXPECT_EQ(SG::num_edge_points(g_reverse_two_edge_point), 2);
    auto edge = boost::edge(0, 1, g_reverse_two_edge_point);
    auto ed = edge.first;
    SG::SplitEdge<GraphType> splitEdge =
        SG::split_edge(e15, ed, g_reverse_two_edge_point);
    EXPECT_TRUE(splitEdge.point_exist_in_edge);
    EXPECT_EQ(splitEdge.edge_point_index, 0);
    std::cout << "AFTER" << std::endl;
    SG::print_spatial_edges(g_reverse_two_edge_point);
    EXPECT_EQ(boost::num_vertices(g_reverse_two_edge_point), 3);
    EXPECT_EQ(boost::num_edges(g_reverse_two_edge_point), 2);
    auto num_ep = SG::num_edge_points(g_reverse_two_edge_point);
    EXPECT_EQ(num_ep, 1);
}
