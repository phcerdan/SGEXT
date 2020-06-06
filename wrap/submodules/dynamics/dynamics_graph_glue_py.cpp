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
