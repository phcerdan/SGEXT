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

#include "dynamics_common_fixtures.hpp"
#include "integrator.hpp"
#include "bonded_forces.hpp"

struct IntegratorPairBondForce_Fixture : public ::testing::Test {
    SG::System sys = SG::System4Fixture();
    SG::IntegratorTwoStep integrator = SG::IntegratorTwoStep(sys);
    void SetUp() override {
        print(sys.bonds, std::cout);
    }
};

TEST_F(IntegratorPairBondForce_Fixture, IntegratorWorks) {
    std::cout << "IntegratorPairBondForce_Fixture: IntegratorWorks"
              << std::endl;
    // Check System for sanity
    // EXPECT_EQ(sys.all.particles.size(), 4);
    // EXPECT_EQ(sys.bonds.collection.size(), 4);
    auto pair_force = integrator.add_force(
            std::make_shared<SG::PairBondForce>(SG::PairBondForce(sys)));
    pair_force->force_function = [](const SG::Particle &a,
                                    const SG::Particle &b) {
        const auto d_ete = ArrayUtilities::minus(b.pos, a.pos); //F_{a, b}
        const auto d_ete_modulo = ArrayUtilities::norm(d_ete);
        // TODO, add parameter for bond:
        const double l_contour_length = 100;
        const double l_persistence = 1000;
        const double relative_extension = d_ete_modulo / l_contour_length;
        const double monomer_anisotropy_inverse = 1 / l_persistence;
        const auto force = SG::force_extension_ev_wlc_normalized(relative_extension, monomer_anisotropy_inverse);
        return ArrayUtilities::product_scalar(d_ete, force/d_ete_modulo );
    };

    const double deltaT = 0.4;
    integrator.integrator_method =
            std::make_shared<SG::VerletVelocitiesIntegratorMethod>(sys, deltaT);
    // set forces

    std::cout << "-------- STARTING SYSTEM --------" << std::endl;
    SG::print(sys.all, std::cout);
    integrator.update(1);
    std::cout << "-------- AFTER UPDATE (0) --------" << std::endl;
    SG::print(sys.all, std::cout);
    const size_t nupdates = 10;
    const size_t particle_index = 2;
    for(size_t n = 1; n < nupdates; ++n){
        std::cout << "-------- Particle[" << particle_index << "]: AFTER UPDATE (" << n << ") --------" << std::endl;
        integrator.update(n);
        SG::print_trajectory(sys.all.particles[particle_index], std::cout);
    }
}
