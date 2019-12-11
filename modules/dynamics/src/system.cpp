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

std::vector<Bond> unique_bonds(const System &sys) {
    std::vector<Bond> bonds;
    for (const auto &particle_neighbor : sys.conexions.collection) {
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


