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

#include "particle_collection.hpp"
#include <algorithm>

namespace SG {

void ParticleCollection::sort() {
    std::sort(std::begin(particles), std::end(particles));
    sorted = true;
};

std::vector<Particle>::iterator ParticleCollection::binary_find(const size_t &id_value) {
    return binary_find(std::begin(particles), std::end(particles), id_value);
}
std::vector<Particle>::const_iterator
ParticleCollection::binary_find(const size_t &id_value) const {
    return binary_find(std::cbegin(particles), std::cend(particles), id_value);
}

std::pair<std::vector<Particle>::const_iterator, size_t>
ParticleCollection::find_particle_and_index(const size_t &id_value) const {
    const auto it = binary_find(id_value);
    if (it != particles.end()) {
        return std::make_pair(it, std::distance(particles.begin(), it));
    } else {
        return std::make_pair(it, std::numeric_limits<size_t>::max());
    }
}

std::pair<std::vector<Particle>::iterator, size_t>
ParticleCollection::find_particle_and_index(const size_t &id_value) {
    const auto it = binary_find(id_value);
    if (it != particles.end()) {
        return std::make_pair(it, std::distance(particles.begin(), it));
    } else {
        return std::make_pair(it, std::numeric_limits<size_t>::max());
    }
}

size_t ParticleCollection::find_index(const size_t &id_value) const {
    const auto it = binary_find(id_value);
    if (it != particles.end()) {
        return std::distance(particles.begin(), it);
    } else {
        return std::numeric_limits<size_t>::max();
    }
}


void print_end_collection(const ParticleCollection &collection,
                                 std::ostream &os) {
    os << "All " << collection.particles.size() << " particles printed"
       << std::endl;
}
void print_id_pos(const ParticleCollection &collection,
                         std::ostream &os) {
    for (const auto &p : collection.particles) {
        print_id_pos(p, os);
        os << "-------------------------------" << std::endl;
    }
    print_end_collection(collection, os);
}
void print(const ParticleCollection &collection, std::ostream &os) {
    for (const auto &p : collection.particles) {
        print(p, os);
        os << "-------------------------------" << std::endl;
    }
    print_end_collection(collection, os);
}

void dump_csv_header(const ParticleCollection &all_particles,
                            std::ostream &os,
                            bool add_end_of_line,
                            bool with_particle_id) {
    const bool internal_do_not_add_end_of_line = false;
    dump_csv_header(all_particles.particles[0], os,
                    internal_do_not_add_end_of_line);
    if (add_end_of_line)
        os << std::endl;
}
void dump_csv_data(const ParticleCollection &all_particles,
                          std::ostream &os,
                          bool add_end_of_line,
                          bool with_particle_id) {
    const bool internal_add_end_of_line = true;
    for (const auto &particle_neighbors : all_particles.particles) {
        dump_csv_data(particle_neighbors, os, internal_add_end_of_line);
    }
    if (add_end_of_line)
        os << std::endl;
}
void dump_csv(const ParticleCollection &all_particles,
                     std::ostream &os) {
    dump_csv_header(all_particles, os);
    dump_csv_data(all_particles, os);
}
} // namespace SG
