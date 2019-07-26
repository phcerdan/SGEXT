/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "spatial_graph_utilities.hpp"
#include "degree_viger_generator.hpp"
#include "gmock/gmock.h"

using namespace ::testing;

class RandomGraphFromDegreesFixture : public ::testing::Test {
  public:
    void SetUp() override {}
};

TEST_F(RandomGraphFromDegreesFixture, simplest) {
    const std::vector<int> degree_sequence = {1, 2, 1};
    auto graph = SG::create_graph_from_degree_sequence(degree_sequence);
    SG::print_degrees(graph);
    SG::print_edges(graph);
    EXPECT_EQ(boost::num_vertices(graph), std::size(degree_sequence));
    EXPECT_EQ(boost::num_edges(graph), 2);
}

TEST_F(RandomGraphFromDegreesFixture, fully_connected) {
    const std::vector<int> degree_sequence = {3, 3, 3, 3};
    auto graph = SG::create_graph_from_degree_sequence(degree_sequence);
    SG::print_degrees(graph);
    SG::print_edges(graph);
    EXPECT_EQ(boost::num_vertices(graph), std::size(degree_sequence));
    EXPECT_EQ(boost::num_edges(graph), 6);
}
