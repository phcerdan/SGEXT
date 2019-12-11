/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_SYSTEM_HPP
#define SG_SYSTEM_HPP

#include "bond_collection.hpp"
#include "particle_collection.hpp"
#include "particle_neighbors.hpp"

namespace SG {
/**
 * System is a catch all structure to perform simulations,
 * Classes might need a reference to it in the constructor.
 */
struct System {
    ParticleCollection all;                ///< all particles
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
std::vector<Bond> unique_bonds(const System &sys);

template<typename TBond>
BondCollection make_unique_bonds_from_system_conexions(const System &sys) {
    BondCollection bond_collection;
    bond_collection.sorted = true;
    auto & bonds = bond_collection.bonds;
    for (const auto &particle_neighbor : sys.conexions.collection) {
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
