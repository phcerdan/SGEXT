/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_SYSTEM_HPP
#define SG_SYSTEM_HPP

#include "particle_collection.hpp"
#include "particle_neighbors.hpp"

namespace SG {
/**
 * System is a catch all structure to perform simulations,
 * Classes might need a reference to it in the constructor.
 */
struct System {
    ParticleCollection all;                ///< all particles
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
} // namespace SG
#endif
