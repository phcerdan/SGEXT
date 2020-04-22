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

namespace py = pybind11;
void init_particle(py::module &);
void init_bond(py::module &);
void init_particle_collection(py::module &);
void init_bond_collection(py::module &);
void init_system(py::module &);
void init_vtu_file_io(py::module &);
void init_integrator_methods(py::module &);
void init_forces(py::module &);
void init_force_compute(py::module &);
void init_integrator(py::module &);

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
}
