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
#include "particle_collection.hpp"
#include "sgdynamics_common_py.hpp"


namespace py = pybind11;
using namespace SG;

void init_particle_collection(py::module &m) {
    py::bind_vector<std::vector<Particle>>(m, "VectorParticles");

    py::class_<ParticleCollection>(m, "particle_collection")
        .def(py::init())
        .def_readwrite("particles", &ParticleCollection::particles)
        .def_readwrite("sorted", &ParticleCollection::sorted)
        .def("sort", &ParticleCollection::sort)
        .def("binary_find", [](ParticleCollection & collection, const size_t & id) -> Particle& {
                auto it = collection.binary_find(id);
                return *it;
                }, py::return_value_policy::reference)
        .def("find_index", &ParticleCollection::find_index)
        .def("__str__", [](const ParticleCollection &p) {
                std::stringstream os;
                print(p, os);
                return os.str();
        });
    m.def("print_id_pos", [](const ParticleCollection& collection) {
                std::stringstream os;
                print_id_pos(collection, os);
                return os.str();
            }, "Print collection with just id and pos");
}
