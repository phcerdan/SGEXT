/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
