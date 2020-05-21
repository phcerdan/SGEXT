/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "dynamics_common_data.hpp"
#include "gmock/gmock.h"

namespace SG {

/**
 * Create particles with consecutive indices
 * Slicing is harmless here, it can be used by value in interface
 * when a regular Particle is needed.
 */
struct ParticleConsecutiveId : public Particle {
    // note: inline non-const static requires c++17
    inline static size_t id_counter = 0;
    ParticleConsecutiveId() : Particle() {
        id = id_counter;
        id_counter++;
    }
};

/**
 * Define a system with 4 particles, populating positions and bonds between
 * them. By index:
 *   3
 *   |
 * 0-1-2
 * The id of the particles might be different than the indices for testing
 * purposes.
 */
struct System4Fixture : public System {
    constexpr static size_t nparticles = 4;
    System4Fixture() : System() {
        // for testing purposes start the id_counter on 10.
        ParticleConsecutiveId::id_counter = 10;
        for (size_t i = 0; i < nparticles; i++) {
            all.particles.emplace_back(ParticleConsecutiveId());
        }
        all.particles[0].pos = {0, 0, 0};
        all.particles[1].pos = {1, 0, 0};
        all.particles[2].pos = {2, 0, 0};
        all.particles[3].pos = {1, 1, 0};
        conexions.emplace_back(ParticleNeighbors(
                all.particles[0].id, {all.particles[1].id} // neighbor ids
                ));
        conexions.emplace_back(ParticleNeighbors(
                all.particles[1].id, {all.particles[0].id, all.particles[2].id,
                                      all.particles[3].id} // neighbor ids
                ));
        conexions.emplace_back(
                ParticleNeighbors(all.particles[2].id, {all.particles[1].id}));
        conexions.emplace_back(
                ParticleNeighbors(all.particles[3].id, {all.particles[1].id}));
        all.sort();
        this->bonds =
                SG::make_unique_bonds_from_system_conexions<SG::BondChain>(
                        this);
    }
};

} // namespace SG
