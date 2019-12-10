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

#ifndef SG_PARTICLE_NEIGHBORS
#define SG_PARTICLE_NEIGHBORS

#include "bond.hpp"

#include <memory>
#include <ostream>
#include <vector>

namespace SG {
struct ParticleNeighbors {
    size_t particle_id;            ///< id of the particle
    std::vector<size_t> neighbors; /// ids of the neighbors of the particle

    ParticleNeighbors() = default;
    explicit ParticleNeighbors(const size_t &id) : particle_id(id){};
    ParticleNeighbors(const size_t &id,
                      const std::vector<size_t> &input_neighbors)
            : particle_id(id), neighbors(input_neighbors){};
    ParticleNeighbors(const ParticleNeighbors &) = default;
    ParticleNeighbors(ParticleNeighbors &&) = default;
    ParticleNeighbors &operator=(const ParticleNeighbors &) = default;
    ParticleNeighbors &operator=(ParticleNeighbors &&) = default;
    ~ParticleNeighbors() = default;
};
void print(const ParticleNeighbors &particle_neighbors, std::ostream &os);
void dump_csv_header(const ParticleNeighbors &particle_neighbors,
                     std::ostream &os,
                     bool add_end_of_line = true,
                     bool with_particle_id = true);
void dump_csv_data(const ParticleNeighbors &particle_neighbors,
                   std::ostream &os,
                   bool add_end_of_line = true,
                   bool with_particle_id = true);
void dump_csv(const ParticleNeighbors &particle_neighbors, std::ostream &os);

struct ParticleNeighborsCollection {
    std::vector<ParticleNeighbors> collection;
};

void print(const ParticleNeighborsCollection &all_neighbors, std::ostream &os);
void dump_csv_header(const ParticleNeighborsCollection &all_particle_neighbors,
                     std::ostream &os,
                     bool add_end_of_line = true,
                     bool with_particle_id = true);
void dump_csv_data(const ParticleNeighborsCollection &all_particle_neighbors,
                   std::ostream &os,
                   bool add_end_of_line = true,
                   bool with_particle_id = true);
void dump_csv(const ParticleNeighborsCollection &all_particle_neighbors,
              std::ostream &os);

} // namespace SG
#endif
