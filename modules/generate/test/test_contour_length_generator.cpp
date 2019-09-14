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

#include "contour_length_generator.hpp"
#include "rng.hpp"
#include "gmock/gmock.h"

struct ContourLengthGeneratorFixture : public ::testing::Test {
    void SetUp() override {
        RNG::engine().seed(10);
        graph_ = SG::GraphType(3);
        graph_[0].pos = {{0.0, 0.0, 0.0}};
        graph_[1].pos = {{0.5, 0.0, 0.0}};
        graph_[2].pos = {{0.0, 0.3, 0.0}};
        boost::add_edge(0, 1, SG::SpatialEdge(), graph_);
        boost::add_edge(0, 2, SG::SpatialEdge(), graph_);
    }
    SG::GraphType graph_;
    double k_bending_ = 2.0;
};

TEST_F(ContourLengthGeneratorFixture, perm_works) {
    std::cout << "contour_length_generator works" << std::endl;
    auto cl_gen = SG::contour_length_generator(graph_);
    auto pair_edge_points01_ratio =
            cl_gen.generate_contour_length(
                    graph_[0].pos, graph_[1].pos, k_bending_);
    auto &edge_points01 = pair_edge_points01_ratio.first;
    SG::print_edge_points(edge_points01, std::cout);
}
