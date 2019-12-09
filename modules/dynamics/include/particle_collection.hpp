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

#ifndef SG_PARTICLE_COLLECTION_HPP
#define SG_PARTICLE_COLLECTION_HPP

#include "particle.hpp"
#include <vector>
#include <cassert>
#include <algorithm>

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
        assert(sorted &&
               "Particles not sorted before binary_find. Call sort() first");
        constexpr auto comp = [](const Particle &p, const size_t &value) {
            return p.id < value;
        };
        return std::lower_bound(first, last, id_value, comp);
        // return (it != last && !comp(*it, id_value) ) ? it : last;
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
