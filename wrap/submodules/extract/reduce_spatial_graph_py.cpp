/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// #include "sgextract_common_py.hpp"
#include "reduce_spatial_graph_via_dfs.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace SG;

void init_reduce_spatial_graph(py::module &m) {
    m.def("reduce_spatial_graph_via_dfs", &reduce_spatial_graph_via_dfs);
    m.def("reduce_spatial_graph", &reduce_spatial_graph_via_dfs);
}
