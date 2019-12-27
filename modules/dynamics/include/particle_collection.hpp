/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_PARTICLE_COLLECTION_HPP
#define SG_PARTICLE_COLLECTION_HPP

#include "particle.hpp"
#include <algorithm>
#include <cassert>
#include <vector>

namespace SG {
/**
 * Simplest ParticleCollection.
 * No multi-threaded, no GPU.
 * For optimizations, have a look at HOOMD ParticleData and friends.
 * That includes spatial optimizations (trees) and multi-threading,
 * including GPU acceleration.
 * NOTE(phcerdan): Too early to optimize, but it would be possible to copy
 * from HOOMD if needed.
 * The main drawback (but I might be wrong) is that HOOMD's ParticleData
 * seems to have a fixed num of particles, at least in MD.
 */
struct ParticleCollection {
    std::vector<Particle> particles;
    bool sorted = false;
    void sort();

    template <typename ForwardIt>
    inline ForwardIt binary_find(const ForwardIt &first,
                                 const ForwardIt &last,
                                 const size_t &id_value) const {
        if (!sorted) {
            throw std::runtime_error(
                    "Particles not sorted in ParticleCollection before a find. "
                    "Call sort() first.");
        }
        constexpr auto comp = [](const Particle &p, const size_t &value) {
            return p.id < value;
        };
        const auto lower_bound_search_it = std::lower_bound(first, last, id_value, comp);

        if (lower_bound_search_it == last ||
            id_value < (*lower_bound_search_it).id) {
            return last;
        }
        return lower_bound_search_it;
    }
    std::vector<Particle>::iterator binary_find(const size_t &id_value);

    std::vector<Particle>::const_iterator
    binary_find(const size_t &id_value) const;

    std::pair<std::vector<Particle>::const_iterator, size_t>
    find_particle_and_index(const size_t &id_value) const;
    std::pair<std::vector<Particle>::iterator, size_t>
    find_particle_and_index(const size_t &id_value);

    size_t find_index(const size_t &id_value) const;
    friend void print(const ParticleCollection &collection);
};

void print_end_collection(const ParticleCollection &collection,
                          std::ostream &os);
void print_id_pos(const ParticleCollection &collection, std::ostream &os);
void print(const ParticleCollection &collection, std::ostream &os);
void dump_csv_header(const ParticleCollection &all_particles,
                     std::ostream &os,
                     bool add_end_of_line = true,
                     bool with_particle_id = true);
void dump_csv_data(const ParticleCollection &all_particles,
                   std::ostream &os,
                   bool add_end_of_line = true,
                   bool with_particle_id = true);
void dump_csv(const ParticleCollection &all_particles, std::ostream &os);
} // namespace SG
#endif
