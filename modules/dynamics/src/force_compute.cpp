/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "force_compute.hpp"

namespace SG {
void PairBondForce::compute() {
    if (!force_function) {
        throw std::runtime_error("force_function is not set in PairBondForce");
    }
    for (auto &p : conexions.collection) {
        // Find the particle index
        const auto [particle_it, particle_index] =
                m_sys.all.find_particle_and_index(p.particle_id);
        if (particle_index == std::numeric_limits<size_t>::max()) {
            throw std::runtime_error("The particle in bonds does not exist "
                                     "in all particles");
        }
        const auto &current_particle = *particle_it;
        // Apply force_function between current particle and its neighbors
        // if there is a proper bond between them
        for (auto &neighbor_id : p.neighbors) {
            const auto [neighbor_particle_it, neighbor_particle_index] =
                    m_sys.all.find_particle_and_index(neighbor_id);
            const auto &current_neighbor = *neighbor_particle_it;
            auto &current_particle_force = forces[particle_index];
            // TODO filter by bond type
            // Find bond
            BondChain chain = {1, 2, 2.0};
            current_particle_force = ArrayUtilities::plus(
                    current_particle_force,
                    force_function(current_particle, current_neighbor, chain));
        }
    }
}
}; // namespace SG
