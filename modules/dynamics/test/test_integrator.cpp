/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
            // TODO, add parameter for bond:
            // const double l_contour_length = 100;
            const auto &l_contour_length =
                    static_cast<const SG::BondChain &>(chain).length_contour;
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

TEST_F(IntegratorPairBondForce_Fixture, test_unique_bonds) {
    auto bonds = SG::unique_bonds(sys);
    for (const auto &bond : bonds) {
        print(bond, std::cout);
    }
}
