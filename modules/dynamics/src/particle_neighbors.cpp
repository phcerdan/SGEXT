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
    for (const auto &p : all_neighbors.collection) {
        print(p, os);
        os << "--" << std::endl;
    }
}
void dump_csv_header(const ParticleNeighborsCollection &all_particle_neighbors,
                     std::ostream &os,
                     bool add_end_of_line,
                     bool with_particle_id) {
    const bool internal_add_end_of_line = true;
    dump_csv_header(all_particle_neighbors.collection[0], os, with_particle_id,
                    internal_add_end_of_line);
    if (add_end_of_line)
        os << std::endl;
}
void dump_csv_data(const ParticleNeighborsCollection &all_particle_neighbors,
                   std::ostream &os,
                   bool add_end_of_line,
                   bool with_particle_id) {
    const bool internal_add_end_of_line = true;
    for (const auto &particle_neighbors : all_particle_neighbors.collection) {
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
