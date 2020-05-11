/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

namespace py = pybind11;
void init_analyze_graph(py::module &);
void init_thin(py::module &);
void init_create_distance_map(py::module &);
void init_mask_image(py::module &);
void init_fill_holes(py::module &);
void init_visualize_spatial_graph(py::module &);

void init_sgscripts(py::module & mparent) {
    auto m = mparent.def_submodule("scripts");
    m.doc() = "Scripts submodule "; // optional module docstring
    init_analyze_graph(m);
    init_thin(m);
    init_create_distance_map(m);
    init_mask_image(m);
    init_fill_holes(m);
    init_visualize_spatial_graph(m);
}
