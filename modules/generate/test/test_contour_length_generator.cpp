/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
