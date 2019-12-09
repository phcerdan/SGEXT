/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
