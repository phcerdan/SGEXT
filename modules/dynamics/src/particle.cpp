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

#include "particle.hpp"

namespace SG {

/**************************************/
/******* ParticleMaterial ********/
/**************************************/

void print(const ParticleMaterial &p, std::ostream &os) {
    os << "radius = " << p.radius << std::endl;
    os << "volume = " << p.volume << std::endl;
    os << "mass = " << p.mass << std::endl;
}

void dump_csv_header(const ParticleMaterial & /*p*/,
                     std::ostream &os,
                     bool add_end_of_line) {
    os << "radius, volume, mass";
    if (add_end_of_line) {
        os << std::endl;
    }
}

void dump_csv_data(const ParticleMaterial &p,
                   std::ostream &os,
                   bool add_end_of_line) {
    os << p.radius << ", ";
    os << p.volume << ", ";
    os << p.mass;
    if (add_end_of_line) {
        os << std::endl;
    }
}
void dump_csv(const ParticleMaterial &p, std::ostream &os) {
    dump_csv_header(p, os);
    dump_csv_data(p, os);
}

/**************************************/
/****** ParticleDynamicProperties *****/
/**************************************/

void print(const ParticleDynamicProperties &p, std::ostream &os) {
    os << "vel = " << ArrayUtilities::to_string(p.vel) << std::endl;
    os << "acc = " << ArrayUtilities::to_string(p.acc) << std::endl;
    os << "net_force = " << ArrayUtilities::to_string(p.net_force) << std::endl;
}
void dump_csv_header(const ParticleDynamicProperties & /*p*/,
                     std::ostream &os,
                     bool add_end_of_line) {
    os << "vel.x, vel.y, vel.z, acc.x, acc.y, acc.z, net_force.x, net_force.y, "
          "net_force.z";
    if (add_end_of_line) {
        os << std::endl;
    }
}
void dump_csv_data(const ParticleDynamicProperties &p,
                   std::ostream &os,
                   bool add_end_of_line) {
    const bool comma_separated = true;
    os << ArrayUtilities::to_string(p.vel, comma_separated);
    os << ", ";
    os << ArrayUtilities::to_string(p.acc, comma_separated);
    os << ", ";
    os << ArrayUtilities::to_string(p.net_force, comma_separated);
    if (add_end_of_line) {
        os << std::endl;
    }
}
void dump_csv(const ParticleDynamicProperties &p, std::ostream &os) {
    dump_csv_header(p, os);
    dump_csv_data(p, os);
}

/**************************************/
/******* Particle *********************/
/**************************************/

void print_id_pos(const Particle &p, std::ostream &os) {
    os << "id = " << p.id << std::endl;
    os << "pos = " << ArrayUtilities::to_string(p.pos) << std::endl;
}
void print(const Particle &p, std::ostream &os) {
    print_id_pos(p, os);
    print(p.dynamics, os);
    print(p.material, os);
}
void print_trajectory(const Particle &p, std::ostream &os) {
    print_id_pos(p, os);
    print(p.dynamics, os);
}

void dump_csv_header(const Particle &p,
                     std::ostream &os,
                     bool add_end_of_line) {
    const bool internal_do_not_add_end_of_line = false;
    os << "id, pos.x, pos.y, pos.z";
    os << ", ";
    dump_csv_header(p.dynamics, os, internal_do_not_add_end_of_line);
    os << ", ";
    dump_csv_header(p.material, os, internal_do_not_add_end_of_line);
    if (add_end_of_line) {
        os << std::endl;
    }
}
void dump_csv_data(const Particle &p, std::ostream &os, bool add_end_of_line) {
    const bool comma_separated = true;
    const bool internal_do_not_add_end_of_line = false;
    os << p.id;
    os << ", ";
    os << ArrayUtilities::to_string(p.pos, comma_separated);
    os << ", ";
    dump_csv_data(p.dynamics, os, internal_do_not_add_end_of_line);
    os << ", ";
    dump_csv_data(p.material, os, internal_do_not_add_end_of_line);
    if (add_end_of_line) {
        os << std::endl;
    }
}
void dump_csv(const Particle &p, std::ostream &os) {
    dump_csv_header(p, os);
    dump_csv_data(p, os);
}

} // namespace SG
