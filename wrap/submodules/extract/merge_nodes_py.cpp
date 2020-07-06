/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
// #include "sgextract_common_py.hpp"
#include "merge_nodes.hpp"

namespace py = pybind11;
using namespace SG;

void init_merge_nodes(py::module &m) {
    m.def("merge_three_connected_nodes", &merge_three_connected_nodes);
    m.def("merge_four_connected_nodes", &merge_four_connected_nodes);
    m.def("merge_two_three_connected_nodes", &merge_two_three_connected_nodes);
    m.def("remove_parallel_edges", &remove_parallel_edges,
          R"(
Use @ref get_parallel_edges to remove the edges of the input graph.
Returns a new copy of the graph.

Parameters:
----------
graph: GraphType
 input spatial graph
keep_larger_spatial_edge: Bool [false by default]
 keep the parallel edge with largest contour length,
 if false, keep shorter parallel edges.
)",
          py::arg("graph"), py::arg("keep_larger_spatial_edge") = false);
}
