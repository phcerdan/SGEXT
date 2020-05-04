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

#include "pybind11_common.h"
#include "sgdynamics_common_py.hpp"
#include "particle.hpp"

namespace py = pybind11;
using namespace SG;

void init_particle(py::module &m) {
    py::class_<ParticleMaterial>(m, "particle_material")
        .def(py::init())
        .def_readwrite("radius", &ParticleMaterial::radius)
        .def_readwrite("volume", &ParticleMaterial::volume)
        .def_readwrite("mass", &ParticleMaterial::mass)
        .def("__str__", [](const ParticleMaterial &p) {
                std::stringstream os;
                print(p, os);
                return os.str();
        });

    py::class_<ParticleDynamicProperties>(m, "particle_dynamics")
        .def(py::init())
        .def_readwrite("vel", &ParticleDynamicProperties::vel)
        .def_readwrite("acc", &ParticleDynamicProperties::acc)
        .def_readwrite("net_force", &ParticleDynamicProperties::net_force)
        .def("__str__", [](const ParticleDynamicProperties &p) {
                std::stringstream os;
                print(p, os);
                return os.str();
                });

    py::class_<Particle>(m, "particle")
        .def(py::init())
        .def_readwrite("id", &Particle::id)
        .def_readwrite("pos", &Particle::pos)
        .def("set_pos", [](Particle &p,
                    size_t index,
                    const ArrayUtilities::Array3D::value_type & in_value
                    ) {
                p.pos[index] = in_value;
                },
        "p.pos[0] = 3.0 does not modify the position of the particle. "
        "Use p.set_pos(0, 3.0) instead if only one index needs to be modified. "
        "Note that changing the whole position works as expected:\n"
        "p.pos = [3,4,5] or p.pos = p2.pos"
                )
        .def_readwrite("dynamics", &Particle::dynamics)
        .def_readwrite("material", &Particle::material)
        .def("__str__", [](const Particle &p) {
                std::stringstream os;
                print(p, os);
                return os.str();
        });
}
