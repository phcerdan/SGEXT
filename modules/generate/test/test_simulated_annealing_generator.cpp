#include "rng.hpp"
#include "gmock/gmock.h"

#include "simulated_annealing_generator.hpp"
#include <iostream>

struct SimulatedAnnealingGeneratorFixture : public ::testing::Test {

    void SetUp() override { RNG::engine().seed(10); }
};

TEST_F(SimulatedAnnealingGeneratorFixture, default_constructor) {
    std::cout << "simulated_annealing works" << std::endl;
    auto gen = SG::simulated_annealing_generator();
}
TEST_F(SimulatedAnnealingGeneratorFixture, num_vertices_constructor_works) {
    std::cout << "simulated_annealing works" << std::endl;
    auto gen = SG::simulated_annealing_generator(1000);
    std::cout << "Engine starting in test" << std::endl;
    gen.engine();
    gen.transition_parameters.print(std::cout);
    gen.print(std::cout);
    gen.histo_cosines_.PrintBreaksAndCounts(std::cout);
    gen.histo_ete_distances_.PrintBreaksAndCounts(std::cout);
}
