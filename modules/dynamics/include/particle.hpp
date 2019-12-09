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

#ifndef SG_PARTICLE_HPP
#define SG_PARTICLE_HPP
#include "array_utilities.hpp"
#include <iostream>

namespace SG {
struct ParticleMaterial {
    double radius = 1.0;
    double volume = 1.0;
    double mass = 1.0;
};
void print(const ParticleMaterial &p, std::ostream &os);
void dump_csv_header(const ParticleMaterial &p,
                     std::ostream &os,
                     bool add_end_of_line = true);
void dump_csv_data(const ParticleMaterial &p,
                   std::ostream &os,
                   bool add_end_of_line = true);
void dump_csv(const ParticleMaterial &p, std::ostream &os);

struct ParticleDynamicProperties {
    ArrayUtilities::Array3D vel;
    ArrayUtilities::Array3D acc;
    ArrayUtilities::Array3D net_force;
};
void print(const ParticleDynamicProperties &p, std::ostream &os);
void dump_csv_header(const ParticleDynamicProperties &p,
                     std::ostream &os,
                     bool add_end_of_line = true);
void dump_csv_data(const ParticleDynamicProperties &p,
                   std::ostream &os,
                   bool add_end_of_line = true);
void dump_csv(const ParticleDynamicProperties &p, std::ostream &os);

struct Particle {
    size_t id;
    ArrayUtilities::Array3D pos;
    ParticleDynamicProperties dynamics;
    ParticleMaterial material;
};
inline bool operator<(const Particle &lhs, const Particle &rhs) {
    return lhs.id < rhs.id;
}

void print_id_pos(const Particle &p, std::ostream &os);
void print(const Particle &p, std::ostream &os);
void print_trajectory(const Particle &p, std::ostream &os);

void dump_csv_header(const Particle &p,
                     std::ostream &os,
                     bool add_end_of_line = true);
void dump_csv_data(const Particle &p,
                   std::ostream &os,
                   bool add_end_of_line = true);
void dump_csv(const Particle &p, std::ostream &os);

} // namespace SG
#endif
