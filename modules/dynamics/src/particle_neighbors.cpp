/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "particle_neighbors.hpp"
#include <algorithm>

namespace SG {

void print(const ParticleNeighbors &particle_neighbors, std::ostream &os) {
    os << "neighbors of particle: " << particle_neighbors.particle_id
       << std::endl;
    os << "neighbors: { ";
    for (const auto &neigh : particle_neighbors.neighbors) {
        os << neigh << ", ";
    }
    os << " }" << std::endl;
};

void dump_csv_header(const ParticleNeighbors &particle_neighbors,
                     std::ostream &os,
                     bool add_end_of_line,
                     bool with_particle_id) {
    if (with_particle_id)
        os << "id, ";
    os << "neighbors";
    if (add_end_of_line)
        os << std::endl;
}
void dump_csv_data(const ParticleNeighbors &particle_neighbors,
                   std::ostream &os,
                   bool add_end_of_line,
                   bool with_particle_id) {
    if (with_particle_id) {
        os << particle_neighbors.particle_id;
        os << ",";
    }
    os << "\"[";
    for (const auto &neigh : particle_neighbors.neighbors) {
        os << neigh << ", ";
    }
    os << "]\"";
    if (add_end_of_line)
        os << std::endl;
}
void dump_csv(const ParticleNeighbors &particle_neighbors, std::ostream &os) {
    dump_csv_header(particle_neighbors, os);
    dump_csv_data(particle_neighbors, os);
}

/**************************************/
/**** ParticleNeighborsCollection *****/
/**************************************/

void print(const ParticleNeighborsCollection &all_neighbors, std::ostream &os) {
    for (const auto &p : all_neighbors) {
        print(p, os);
        os << "--" << std::endl;
    }
}
void dump_csv_header(const ParticleNeighborsCollection &all_particle_neighbors,
                     std::ostream &os,
                     bool add_end_of_line,
                     bool with_particle_id) {
    const bool internal_add_end_of_line = true;
    dump_csv_header(all_particle_neighbors[0], os, with_particle_id,
                    internal_add_end_of_line);
    if (add_end_of_line)
        os << std::endl;
}
void dump_csv_data(const ParticleNeighborsCollection &all_particle_neighbors,
                   std::ostream &os,
                   bool add_end_of_line,
                   bool with_particle_id) {
    const bool internal_add_end_of_line = true;
    for (const auto &particle_neighbors : all_particle_neighbors) {
        dump_csv_data(particle_neighbors, os, internal_add_end_of_line,
                      with_particle_id);
    }
    if (add_end_of_line)
        os << std::endl;
}
void dump_csv(const ParticleNeighborsCollection &all_particle_neighbors,
              std::ostream &os) {
    dump_csv_header(all_particle_neighbors, os);
    dump_csv_data(all_particle_neighbors, os);
}

} // namespace SG
