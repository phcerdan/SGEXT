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
