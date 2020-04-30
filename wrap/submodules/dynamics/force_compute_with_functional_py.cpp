/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include <pybind11/functional.h> // allows wrapping from python to cpp

#include "force_compute.hpp"


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
