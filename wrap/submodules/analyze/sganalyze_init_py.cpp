/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

namespace py = pybind11;
void init_compute_graph_properties(py::module &);

void init_sganalyze(py::module & mparent) {
    auto m = mparent.def_submodule("analyze");
    m.doc() = "Analyze submodule "; // optional module docstring
    init_compute_graph_properties(m);
}
