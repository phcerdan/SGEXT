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
        for (auto &neighbor_id : p.neighbors) {
            const auto [neighbor_particle_it, neighbor_particle_index] =
                    m_sys.all.find_particle_and_index(neighbor_id);
            const auto &current_neighbor = *neighbor_particle_it;
            auto &current_particle_force = forces[particle_index];
            // TODO
            Chain chain = {1, 2, 2.0};
            current_particle_force = ArrayUtilities::plus(
                    current_particle_force,
                    force_function(current_particle, current_neighbor, chain));
        }
    }
}
}; // namespace SG
