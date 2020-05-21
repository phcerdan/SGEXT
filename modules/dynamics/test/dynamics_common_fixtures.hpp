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
