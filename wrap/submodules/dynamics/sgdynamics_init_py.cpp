/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include "sgdynamics_common_py.hpp"

namespace py = pybind11;
void init_particle(py::module &);
void init_bond(py::module &);
void init_particle_collection(py::module &);
void init_bond_collection(py::module &);
void init_system(py::module &);
void init_integrator_methods(py::module &);
void init_forces(py::module &);
void init_force_compute(py::module &);
void init_integrator(py::module &);
void init_dynamics_graph_glue(py::module &);
#ifdef SG_USING_VTK
void init_vtu_file_io(py::module &);
#endif

void init_sgdynamics(py::module & mparent) {
    auto m = mparent.def_submodule("dynamics");
    m.doc() = "Dynamics submodule"; // optional module docstring
    init_particle(m);
    init_bond(m);
    init_particle_collection(m);
    init_bond_collection(m);
    init_system(m);
    init_integrator_methods(m);
    init_forces(m);
    init_force_compute(m);
    init_integrator(m);
    init_dynamics_graph_glue(m);
#ifdef SG_USING_VTK
    init_vtu_file_io(m);
#endif
}
