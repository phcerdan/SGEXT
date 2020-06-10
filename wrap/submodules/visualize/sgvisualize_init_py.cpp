/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

namespace py = pybind11;

void init_convert_to_vtk_unstructured_grid(py::module &);

void init_sgvisualize(py::module & mparent) {
    auto m = mparent.def_submodule("visualize");
    m.doc() = "Visualize submodule"; // optional module docstring
    init_convert_to_vtk_unstructured_grid(m);
}
