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

#ifndef SG_SYSTEM_HPP
#define SG_SYSTEM_HPP

#include <memory> // for std::enable_shared_from_this

#include "bond_collection.hpp"
#include "particle_collection.hpp"
#include "particle_neighbors.hpp"

namespace SG {
/**
 * System is a catch all structure to perform simulations,
 * Classes might need a pointer to it in the constructor.
 */
struct System : public std::enable_shared_from_this<System> {
    ParticleCollection all; ///< all particles
    BondCollection bonds;
    ParticleNeighborsCollection conexions; ///< fixed bonds between particles
    /** Dynamic neighbors per particle based on positions. */
    ParticleNeighborsCollection collision_neighbor_list;
    // Helpers to get references of data.
    ArrayUtilities::Array3D &get_position(size_t index);
    const ArrayUtilities::Array3D &get_position(size_t index) const;
    ArrayUtilities::Array3D &get_velocity(size_t index);
    const ArrayUtilities::Array3D &get_velocity(size_t index) const;
    ArrayUtilities::Array3D &get_acceleration(size_t index);
    const ArrayUtilities::Array3D &get_acceleration(size_t index) const;
    /// Return copy of positions
    auto all_positions_copy();
    /// Return copy of velocities
    auto all_velocities_copy();
    /// Return copy of accelerations
    auto all_accelerations_copy();
};

/**
 * Get unique bonds from conexions
 *
 * @param sys
 *
 * @return vector with unique Bonds
 */
std::vector<Bond> unique_bonds(const System *sys);

template <typename TBond>
BondCollection make_unique_bonds_from_system_conexions(const System *sys) {
    BondCollection bond_collection;
    bond_collection.sorted = true;
    auto &bonds = bond_collection.bonds;
    for (const auto &particle_neighbor : sys->conexions) {
        const auto source_particle_id = particle_neighbor.particle_id;
        for (const auto &neigh : particle_neighbor.neighbors) {
            auto bond_ptr = std::make_shared<TBond>(source_particle_id, neigh);
            sort(*bond_ptr);
            auto found = bond_collection.find_bond(*bond_ptr);
            if (found == std::end(bonds)) {
                bonds.push_back(bond_ptr);
                bond_collection.sort();
            }
        }
    }
    return bond_collection;
}
} // namespace SG
#endif
