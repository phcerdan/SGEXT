/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "system.hpp"
namespace SG {

ArrayUtilities::Array3D &System::get_position(size_t index) {
    return all.particles[index].pos;
}
const ArrayUtilities::Array3D &System::get_position(size_t index) const {
    return all.particles[index].pos;
}
ArrayUtilities::Array3D &System::get_velocity(size_t index) {
    return all.particles[index].dynamics.vel;
}
const ArrayUtilities::Array3D &System::get_velocity(size_t index) const {
    return all.particles[index].dynamics.vel;
}
ArrayUtilities::Array3D &System::get_acceleration(size_t index) {
    return all.particles[index].dynamics.acc;
}
const ArrayUtilities::Array3D &System::get_acceleration(size_t index) const {
    return all.particles[index].dynamics.acc;
}
auto System::all_positions_copy() {
    const auto nparts = all.particles.size();
    std::vector<ArrayUtilities::Array3D> positions(nparts);
    for (unsigned int i = 0; i < nparts; ++i) {
        positions[i] = all.particles[i].pos;
    }
    return positions;
}
auto System::all_velocities_copy() {
    const auto nparts = all.particles.size();
    std::vector<ArrayUtilities::Array3D> velocities(nparts);
    for (unsigned int i = 0; i < nparts; ++i) {
        velocities[i] = all.particles[i].dynamics.vel;
    }
    return velocities;
}
auto System::all_accelerations_copy() {
    const auto nparts = all.particles.size();
    std::vector<ArrayUtilities::Array3D> accelerations(nparts);
    for (unsigned int i = 0; i < nparts; ++i) {
        accelerations[i] = all.particles[i].dynamics.acc;
    }
    return accelerations;
}

std::vector<Bond> unique_bonds(const System *sys) {
    std::vector<Bond> bonds;
    for (const auto &particle_neighbor : sys->conexions) {
        const auto source_particle_id = particle_neighbor.particle_id;
        for (const auto &neigh : particle_neighbor.neighbors) {
            Bond bond = {source_particle_id, neigh};
            sort(bond);
            auto found = std::find(std::begin(bonds), std::end(bonds), bond);
            if (found == std::end(bonds)) {
                bonds.push_back(bond);
            }
        }
    }
    return bonds;
}
} // namespace SG
