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
    // for testing purposes start the id_counter on 10.
    static size_t id_counter = 10;
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
        for (size_t i = 0; i < nparticles; i++) {
            all.particles.emplace_back(ParticleConsecutiveId());
        }
        all.particles[0].pos = {0, 0, 0};
        all.particles[1].pos = {1, 0, 0};
        all.particles[2].pos = {2, 0, 0};
        all.particles[3].pos = {1, 1, 0};
        bonds.collection.emplace_back(ParticleNeighbors(
                all.particles[0].id, {all.particles[1].id} // neighbor ids
                ));
        bonds.collection.emplace_back(ParticleNeighbors(
                all.particles[1].id, {all.particles[0].id, all.particles[2].id,
                                      all.particles[3].id} // neighbor ids
                ));
        bonds.collection.emplace_back(
                ParticleNeighbors(all.particles[2].id, {all.particles[1].id}));
        bonds.collection.emplace_back(
                ParticleNeighbors(all.particles[3].id, {all.particles[1].id}));
    }
};

} // namespace SG
