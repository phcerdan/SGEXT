/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include "sglocate_common.h" // define holder for vtkPoints

namespace py = pybind11;
void init_get_vtk_points_from_graph(py::module &m);
void init_graph_points_locator(py::module &m);

void init_sglocate(py::module & mparent) {
    auto m = mparent.def_submodule("locate");
    m.doc() = "Locate submodule "; // optional module docstring
    init_get_vtk_points_from_graph(m);
    init_graph_points_locator(m);
}
