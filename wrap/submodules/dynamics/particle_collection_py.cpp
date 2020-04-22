/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
