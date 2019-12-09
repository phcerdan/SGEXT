/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
