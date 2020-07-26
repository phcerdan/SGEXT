/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rng.hpp"
#include "gmock/gmock.h"

#include "simulated_annealing_generator.hpp"
#include "spatial_graph.hpp"
#include <boost/graph/graphviz.hpp>
#include <iostream>

struct SimulatedAnnealingGeneratorFixture : public ::testing::Test {

    void SetUp() override {
        RNG::engine().seed(10);
        graph_ = SG::GraphType(4);
        graph_[0].pos = {{0.25, 0.25, 0.25}};
        graph_[1].pos = {{0.5, 0.25, 0.25}};
        graph_[2].pos = {{0.25, 0.5, 0.25}};
        graph_[3].pos = {{0.25, 0.9, 0.25}};
        boost::add_edge(0, 1, SG::SpatialEdge(), graph_);
        boost::add_edge(0, 2, SG::SpatialEdge(), graph_);
        boost::add_edge(2, 3, SG::SpatialEdge(), graph_);
    }
    SG::GraphType graph_;
};

TEST_F(SimulatedAnnealingGeneratorFixture, default_constructor) {
    std::cout << "simulated_annealing works" << std::endl;
    auto gen = SG::simulated_annealing_generator();
}
TEST_F(SimulatedAnnealingGeneratorFixture, cosines_works) {
    auto gen = SG::simulated_annealing_generator(graph_);
    const size_t num_bins_ete_distances = 100;
    const size_t num_bins_cosines = 100;
    gen.init_histograms(num_bins_ete_distances, num_bins_cosines);
    gen.histo_cosines_.PrintBreaksAndCounts(std::cout);
    EXPECT_EQ(gen.histo_cosines_
                      .counts[gen.histo_cosines_.IndexFromValue(0.00000001)],
              1);
    EXPECT_EQ(gen.histo_cosines_
                      .counts[gen.histo_cosines_.IndexFromValue(-0.999)],
              1);
}

TEST_F(SimulatedAnnealingGeneratorFixture, update_step_move_node) {
    auto gen = SG::simulated_annealing_generator(graph_);
    const size_t num_bins_ete_distances = 100;
    const size_t num_bins_cosines = 100;
    gen.init_histograms(num_bins_ete_distances, num_bins_cosines);
    gen.histo_cosines_.PrintBreaksAndCounts(std::cout);
    gen.print(std::cout);
    gen.transition_params.UPDATE_STEP_MOVE_NODE_PROBABILITY = 1.0;
    gen.transition_params.MAX_ENGINE_ITERATIONS = 2;
    gen.engine();
    gen.print(std::cout);
}

// TEST_F(SimulatedAnnealingGeneratorFixture, update_step_swap_edges) {
//     auto gen = SG::simulated_annealing_generator(10);
//     gen.histo_cosines_.PrintBreaksAndCounts(std::cout);
//     gen.print(std::cout);
//     gen.transition_params.UPDATE_STEP_MOVE_NODE_PROBABILITY = 0.0;
//     gen.transition_params.MAX_ENGINE_ITERATIONS = 100;
//     gen.engine();
//     gen.print(std::cout);
// }

TEST_F(SimulatedAnnealingGeneratorFixture, num_vertices_constructor_works) {
    std::cout << "simulated_annealing works" << std::endl;
    auto gen = SG::simulated_annealing_generator(100);
    const size_t num_bins_ete_distances = 100;
    const size_t num_bins_cosines = 100;
    gen.init_histograms(num_bins_ete_distances, num_bins_cosines);
    std::cout << "Engine starting in test" << std::endl;
    gen.transition_params.UPDATE_STEP_MOVE_NODE_PROBABILITY = 0.5;
    gen.transition_params.MAX_ENGINE_ITERATIONS = 1000;
    gen.print(std::cout);
    gen.engine();
    gen.print_histo_and_target_distribution_ete_distances(std::cout);
    gen.print_histo_and_target_distribution_cosines(std::cout);
    gen.print(std::cout);
}

TEST_F(SimulatedAnnealingGeneratorFixture, config_tree_constructor_works) {
    std::cout << "simulated_annealing config tree works" << std::endl;
    auto tree = SG::simulated_annealing_generator_config_tree();
    tree.transition_params.MAX_ENGINE_ITERATIONS = 1000;
    tree.transition_params.ENERGY_CONVERGENCE = 100;
    tree.transition_params.UPDATE_STEP_MOVE_NODE_PROBABILITY = 0.5;
    tree.ete_distance_params.num_bins = 100;
    tree.cosine_params.num_bins = 100;
    auto gen = SG::simulated_annealing_generator(tree);
    gen.engine();
    gen.print(std::cout);
}
