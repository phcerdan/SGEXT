/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "integrator.hpp"
#include "dynamics_common_fixtures.hpp"


struct IntegratorPairBondForce_Fixture : public ::testing::Test {
    SG::System sys = SG::System4Fixture();
    SG::Integrator integrator = SG::Integrator(sys);
    // SG::PairBondForce pair_bond_force = SG::PairBondForce(sys);
    void SetUp() override {
    }
};

TEST_F(IntegratorPairBondForce_Fixture, IntegratorWorks) {
    std::cout << "IntegratorPairBondForce_Fixture: IntegratorWorks" << std::endl;
    // Check System for sanity
    // EXPECT_EQ(sys.all.particles.size(), 4);
    // EXPECT_EQ(sys.bonds.collection.size(), 4);
    auto pair_force = integrator.add_force(
            std::make_shared<SG::PairBondForce>(
                SG::PairBondForce(sys)));
    pair_force->force_function = [](const SG::Particle& a, const SG::Particle &b) {
        return a.pos;
    };
    const double deltaT = 0.4;
    integrator.integrator_method = std::make_shared<SG::VerletVelocitiesMethod>(
            sys, deltaT);
    integrator.update(1);
}
