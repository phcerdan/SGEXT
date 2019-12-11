/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "force_compute.hpp"
#include "particle_collection.hpp"

namespace SG {
void PairBondForce::compute() {
    if (!force_function) {
        throw std::runtime_error("force_function is not set in PairBondForce");
    }

    size_t current_particle_index = 0;
    for (auto &particle : m_sys.all.particles) {
        auto bonds = m_sys.bonds.find_all_bonds_with_id(particle.id);
        for (const auto &bond : bonds) {
            const auto connected_particle_id =
                    (particle.id == bond->id_a) ? bond->id_b : bond->id_a;
            const auto [connected_particle_it, connected_particle_index] =
                    m_sys.all.find_particle_and_index(connected_particle_id);

            auto &current_particle_force =
                    particle_forces[current_particle_index].force;
            assert(particle.id == particle_forces[current_particle_index]
                                          .particle_id &&
                   "particle ids are not synchornized in "
                   "PairBondeForce:compute(), they should be sorted as well as "
                   "all the particles.");
            current_particle_force = ArrayUtilities::plus(
                    current_particle_force,
                    force_function(particle, *connected_particle_it, *bond));
        }
        current_particle_index++;
    }
    // for (auto &p : conexions.collection) {
    //     // Find the particle index
    //     const auto [particle_it, particle_index] =
    //             m_sys.all.find_particle_and_index(p.particle_id);
    //     if (particle_index == std::numeric_limits<size_t>::max()) {
    //         throw std::runtime_error("The particle in bonds does not
    //         exist.");
    //     }
    //     const auto &current_particle = *particle_it;
    //     // Apply force_function between current particle and its neighbors
    //     // if there is a proper bond between them
    //     for (auto &neighbor_id : p.neighbors) {
    //         const auto [neighbor_particle_it, neighbor_particle_index] =
    //                 m_sys.all.find_particle_and_index(neighbor_id);
    //         const auto &current_neighbor = *neighbor_particle_it;
    //         auto &current_particle_force = forces[particle_index];
    //         // TODO filter by bond type
    //         // Find bond
    //         BondChain chain = {1, 2, 2.0};
    //         current_particle_force = ArrayUtilities::plus(
    //                 current_particle_force,
    //                 force_function(current_particle, current_neighbor,
    //                 chain));
    //     }
    // }
}
}; // namespace SG
