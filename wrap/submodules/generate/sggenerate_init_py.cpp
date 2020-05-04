/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

namespace py = pybind11;
void init_simulated_annealing_generator_parameters(py::module &);
void init_simulated_annealing_generator(py::module &);
void init_contour_length_generator(py::module &);

void init_sggenerate(py::module & mparent) {
    auto m = mparent.def_submodule("generate");
    m.doc() = "Generate submodule"; // optional module docstring
    init_simulated_annealing_generator_parameters(m);
    init_simulated_annealing_generator(m);
    init_contour_length_generator(m);
}
