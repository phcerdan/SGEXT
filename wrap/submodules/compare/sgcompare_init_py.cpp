/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

namespace py = pybind11;
void init_extend_low_info_graph(py::module &);
void init_add_graph_peninsula(py::module &);
void init_spatial_graph_difference(py::module &);

void init_sgcompare(py::module & mparent) {
    auto m = mparent.def_submodule("compare");
    m.doc() = "Compare submodule "; // optional module docstring
    init_extend_low_info_graph(m);
    init_add_graph_peninsula(m);
    init_spatial_graph_difference(m);
}
