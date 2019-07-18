#include "gmock/gmock.h"

#include "simulated_annealing.hpp"
#include <iostream>

struct SimulatedAnnealingFixture : public ::testing::Test {
    const double pi = atan(1.0) * 4.0;
};

TEST_F(SimulatedAnnealingFixture, works) {
    std::cout << "simulated_annealing works" << std::endl;
}
