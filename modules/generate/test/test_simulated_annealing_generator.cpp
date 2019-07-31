#include "rng.hpp"
#include "gmock/gmock.h"

#include "simulated_annealing_generator.hpp"
#include "spatial_graph.hpp"
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
    gen.histo_cosines_.PrintBreaksAndCounts(std::cout);
    gen.print(std::cout);
    gen.transition_parameters.UPDATE_STEP_MOVE_NODE_PROBABILITY = 1.0;
    gen.transition_parameters.MAX_ENGINE_ITERATIONS = 2;
    gen.engine();
    gen.print(std::cout);
}

// TEST_F(SimulatedAnnealingGeneratorFixture, update_step_swap_edges) {
//     auto gen = SG::simulated_annealing_generator(10);
//     gen.histo_cosines_.PrintBreaksAndCounts(std::cout);
//     gen.print(std::cout);
//     gen.transition_parameters.UPDATE_STEP_MOVE_NODE_PROBABILITY = 0.0;
//     gen.transition_parameters.MAX_ENGINE_ITERATIONS = 100;
//     gen.engine();
//     gen.print(std::cout);
// }

TEST_F(SimulatedAnnealingGeneratorFixture, num_vertices_constructor_works) {
    std::cout << "simulated_annealing works" << std::endl;
    auto gen = SG::simulated_annealing_generator(100);
    std::cout << "Engine starting in test" << std::endl;
    gen.transition_parameters.UPDATE_STEP_MOVE_NODE_PROBABILITY = 0.5;
    gen.transition_parameters.MAX_ENGINE_ITERATIONS = 1000000;
    gen.print(std::cout);
    gen.engine();
    gen.print(std::cout);
    gen.histo_cosines_.PrintBreaksAndCounts(std::cout);
    gen.histo_ete_distances_.PrintBreaksAndCounts(std::cout);
}
