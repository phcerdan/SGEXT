#include "gmock/gmock.h"

#include "simulated_annealing_generator.hpp"
#include <iostream>

struct SimulatedAnnealingGeneratorFixture : public ::testing::Test {
    const double pi = atan(1.0) * 4.0;
};

TEST_F(SimulatedAnnealingGeneratorFixture, works) {
    std::cout << "simulated_annealing works" << std::endl;
}
