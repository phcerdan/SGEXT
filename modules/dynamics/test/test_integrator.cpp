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

#include "bonded_forces.hpp"
#include "dynamics_common_fixtures.hpp"
#include "integrator.hpp"
#include "write_vtu_file.hpp"
#include <fstream>

struct IntegratorPairBondForce_Fixture : public ::testing::Test {
    SG::System sys = SG::System4Fixture();
    SG::IntegratorTwoStep integrator = SG::IntegratorTwoStep(sys);
    std::shared_ptr<SG::PairBondForce> pair_force;
    double deltaT = 0.4;
    void SetUp() override {
        integrator.integrator_method =
                std::make_shared<SG::VerletVelocitiesIntegratorMethod>(sys,
                                                                       deltaT);
        // create pair_force
        pair_force = integrator.add_force(
                std::make_shared<SG::PairBondForce>(SG::PairBondForce(sys)));
        pair_force->force_function = [](const SG::Particle &a,
                                        const SG::Particle &b,
                                        const SG::Bond &chain) {
            const auto d_ete = ArrayUtilities::minus(b.pos, a.pos); // F_{a, b}
            const auto d_ete_modulo = ArrayUtilities::norm(d_ete);
            const auto &l_contour_length =
                    static_cast<const SG::BondChain &>(chain).length_contour;
            std::cout << "[" << a.id << ", " << b.id
                      << "] lengh_contour: " << l_contour_length << std::endl;
            const double l_persistence = 1000;
            const double relative_extension = d_ete_modulo / l_contour_length;
            const double monomer_anisotropy_inverse = 1 / l_persistence;
            const auto force = SG::force_extension_ev_wlc_normalized(
                    relative_extension, monomer_anisotropy_inverse);
            return ArrayUtilities::product_scalar(d_ete, force / d_ete_modulo);
        };
    }
};

TEST_F(IntegratorPairBondForce_Fixture, IntegratorWorks) {
    // Check System for sanity
    EXPECT_EQ(sys.all.particles.size(), 4);
    EXPECT_EQ(sys.conexions.collection.size(), 4);
    std::cout << "IntegratorPairBondForce_Fixture: IntegratorWorks"
              << std::endl;
}

TEST_F(IntegratorPairBondForce_Fixture, DumpOneParticleTrajectory) {
    std::cout << "IntegratorPairBondForce_Fixture: DumpOneParticleTrajectory"
              << std::endl;
    std::cout << "-------- STARTING SYSTEM --------" << std::endl;
    const size_t time_steps = 10;
    const size_t particle_index = 2;
    const std::string base_file =
            "./trajectory_n" + std::to_string(particle_index) + "_t";
    SG::print(sys.all, std::cout);
    { // time_step = 0
        const size_t time_step = 0;
        const std::string final_file =
                base_file + std::to_string(time_step) + ".csv";
        std::ofstream fout(final_file);
        SG::dump_csv(sys.all, fout);
    }
    for (size_t time_step = 1; time_step < time_steps; ++time_step) {
        const std::string final_file =
                base_file + std::to_string(time_step) + ".csv";
        std::ofstream fout(final_file);
        integrator.update(time_step);
        SG::dump_csv(sys.all, fout);
    }
    std::cout << "-------- FINAL SYSTEM --------" << std::endl;
    SG::print(sys.all, std::cout);
}

TEST_F(IntegratorPairBondForce_Fixture, DumpAllParticlesTrajectory) {
    std::cout << "IntegratorPairBondForce_Fixture: DumpAllParticlesTrajectory"
              << std::endl;
    const size_t time_steps = 10;
    const std::string base_file = "./all_trajectories_t";

    // time_step = 0
    {
        const size_t time_step = 0;
        const std::string final_file =
                base_file + std::to_string(time_step) + ".csv";
        std::ofstream fout(final_file);
        SG::dump_csv(sys.all, fout);
    }
    for (size_t time_step = 1; time_step < time_steps; ++time_step) {
        const std::string final_file =
                base_file + std::to_string(time_step) + ".csv";
        std::ofstream fout(final_file);
        integrator.update(time_step);
        SG::dump_csv(sys.all, fout);
    }
}

TEST_F(IntegratorPairBondForce_Fixture, DumpAllParticlesBonds) {
    std::cout << "IntegratorPairBondForce_Fixture: DumpAllParticlesBonds"
              << std::endl;
    const size_t time_steps = 10;
    const std::string base_file = "./all_bonds_t";

    // time_step = 0
    {
        const size_t time_step = 0;
        const std::string final_file =
                base_file + std::to_string(time_step) + ".csv";
        std::ofstream fout(final_file);
        SG::dump_csv(sys.conexions, fout);
    }
    for (size_t time_step = 1; time_step < time_steps; ++time_step) {
        const std::string final_file =
                base_file + std::to_string(time_step) + ".csv";
        std::ofstream fout(final_file);
        integrator.update(time_step);
        SG::dump_csv(sys.conexions, fout);
    }
}

TEST_F(IntegratorPairBondForce_Fixture, write_wtu_file) {
    std::cout << "IntegratorPairBondForce_Fixture: write_wtu_file" << std::endl;
    const size_t time_steps = 10;
    const std::string base_file = "./system";

    // time_step = 0
    {
        const size_t time_step = 0;
        const std::string final_file =
                base_file + std::to_string(time_step) + ".vtu";
        std::ofstream fout(final_file);
        SG::write_vtu_file(sys, final_file);
    }
    for (size_t time_step = 1; time_step < time_steps; ++time_step) {
        const std::string final_file =
                base_file + std::to_string(time_step) + ".vtu";
        std::ofstream fout(final_file);
        integrator.update(time_step);
        SG::write_vtu_file(sys, final_file);
    }
}

TEST_F(IntegratorPairBondForce_Fixture,
       test_unique_bonds_from_system_conexions) {
    auto bond_collection =
            SG::make_unique_bonds_from_system_conexions<SG::Bond>(sys);
    for (const auto &bond : bond_collection.bonds) {
        print(*bond, std::cout);
    }
    EXPECT_EQ(*bond_collection.bonds[0], SG::Bond(10, 11));
    EXPECT_EQ(*bond_collection.bonds[1], SG::Bond(11, 12));
    EXPECT_EQ(*bond_collection.bonds[2], SG::Bond(11, 13));
}

TEST_F(IntegratorPairBondForce_Fixture, test_unique_bonds) {
    auto bonds = SG::unique_bonds(sys);
    for (const auto &bond : bonds) {
        print(bond, std::cout);
    }
    EXPECT_EQ(bonds[0], SG::Bond(10, 11));
    EXPECT_EQ(bonds[1], SG::Bond(11, 12));
    EXPECT_EQ(bonds[2], SG::Bond(11, 13));
}

TEST_F(IntegratorPairBondForce_Fixture, bonds_with_properties) {
    std::cout << "IntegratorPairBondForce_Fixture: bonds_with_properties" << std::endl;
    const size_t time_steps = 10;
    const std::string base_file = "./system_with_props";

    std::static_pointer_cast<SG::BondChain>(sys.bonds.bonds[0])
            ->length_contour = 100;
    std::static_pointer_cast<SG::BondChain>(sys.bonds.bonds[1])
            ->length_contour = 2000;
    std::static_pointer_cast<SG::BondChain>(sys.bonds.bonds[2])
            ->length_contour = 30000;

    // time_step = 0
    {
        const size_t time_step = 0;
        const std::string final_file =
                base_file + std::to_string(time_step) + ".vtu";
        std::ofstream fout(final_file);
        SG::write_vtu_file(sys, final_file);
    }
    for (size_t time_step = 1; time_step < time_steps; ++time_step) {
        const std::string final_file =
                base_file + std::to_string(time_step) + ".vtu";
        std::ofstream fout(final_file);
        integrator.update(time_step);
        SG::write_vtu_file(sys, final_file);
    }
}
