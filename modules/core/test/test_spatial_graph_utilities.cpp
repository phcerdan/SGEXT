/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gmock/gmock.h"
#include "spatial_graph_utilities.hpp"

struct CoreUtilitiesFixture : public ::testing::Test {
    void SetUp() override {
        this->CreateGraph();
    }
    void CreateGraph() {
        g = SG::GraphType(3);
        p0 = SG::PointType{{0.0, 0.0, 0.0}};
        p1 = SG::PointType{{1.0, 0.0, 0.0}};
        p2 = SG::PointType{{1.0, 1.0, 0.0}};
        g[0].pos = p0;
        g[1].pos = p1;
        g[2].pos = p2;
        boost::add_edge(0, 1, SG::SpatialEdge(), g);
        boost::add_edge(1, 2, SG::SpatialEdge(), g);
    }
    SG::GraphType g;
    SG::PointType p0, p1, p2;
};

TEST_F(CoreUtilitiesFixture, get_adjacent_vertices_positions_works_for_vertex0) {
    auto adjacent_vertices_positions = SG::get_adjacent_vertices_positions(0, g);
    auto descs = adjacent_vertices_positions.neighbours_descriptors;
    EXPECT_EQ(descs.size(), 1);
    auto positions = adjacent_vertices_positions.neighbours_positions;
    EXPECT_EQ(positions.size(), 1);
    EXPECT_EQ(descs[0], 1);
    EXPECT_EQ(positions[0], p1);
}
TEST_F(CoreUtilitiesFixture, get_adjacent_vertices_positions_works_for_vertex1) {
    auto adjacent_vertices_positions = SG::get_adjacent_vertices_positions(1, g);
    auto descs = adjacent_vertices_positions.neighbours_descriptors;
    EXPECT_EQ(descs.size(), 2);
    auto positions = adjacent_vertices_positions.neighbours_positions;
    EXPECT_EQ(positions.size(), 2);
    EXPECT_EQ(descs[0], 0);
    EXPECT_EQ(positions[0], p0);
    EXPECT_EQ(descs[1], 2);
    EXPECT_EQ(positions[1], p2);
}
TEST_F(CoreUtilitiesFixture, get_adjacent_vertices_positions_works_for_vertex2) {
    auto adjacent_vertices_positions = SG::get_adjacent_vertices_positions(2, g);
    auto descs = adjacent_vertices_positions.neighbours_descriptors;
    EXPECT_EQ(descs.size(), 1);
    auto positions = adjacent_vertices_positions.neighbours_positions;
    EXPECT_EQ(positions.size(), 1);
    EXPECT_EQ(descs[0], 1);
    EXPECT_EQ(positions[0], p1);
}

