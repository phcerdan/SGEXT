/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include "sgdynamics_common_py.hpp"

#include "dynamics_graph_glue.hpp"

namespace py = pybind11;
using namespace SG;

void init_dynamics_graph_glue(py::module &m) {
    py::class_<ParticleGraphGlueData>(m, "particle_graph_glue_data")
        .def(py::init())
        .def_readwrite("sys", &ParticleGraphGlueData::sys)
        .def("get_particle_graph_map", [](const ParticleGraphGlueData &obj) {
                return *(obj.particle_graph_map);
                })
        .def("get_graph_particle_map", [](const ParticleGraphGlueData &obj) {
                return *(obj.graph_particle_map);
                })
        ;

    m.def("particles_from_graph", &particles_from_graph);
}
