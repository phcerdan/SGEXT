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

#include "force_compute.hpp"
#include <pybind11/functional.h> // allows wrapping from python to cpp
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace SG;

/**
 * Isolated translation unit to include <pybind11/functional.h>
 * for the only places is needed. Avoiding unnecesary copies all over the place.
 * Reference: https://github.com/pybind/pybind11/issues/11
 */

void wrap_force_function_with_functional(py::class_<ParticleForceCompute> &c) {
    c.def_readwrite("force_function", &ParticleForceCompute::force_function);
}
void wrap_force_function_with_functional(py::class_<PairBondForceWithBond> &c) {
    c.def_readwrite("force_function", &PairBondForceWithBond::force_function);
}
void wrap_force_function_with_functional(py::class_<PairBondForce> &c) {
    c.def_readwrite("force_function", &PairBondForce::force_function);
}
