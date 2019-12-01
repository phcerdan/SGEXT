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
