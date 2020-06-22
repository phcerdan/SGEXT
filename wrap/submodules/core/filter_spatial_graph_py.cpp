/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

#include "filter_spatial_graph.hpp"

namespace py = pybind11;
using namespace SG;

void init_filter_spatial_graph(py::module &m) {
    m.def("largest_connected_component", &copy_largest_connected_component,
            R"(
Create a new graph holding the largest component of the input graph.

Vertex and Edge indices are not shared between the input graph and the output.
See @filter_component_graphs in the C++ side if this is needed.

Parameters:
---------
input_graph: GraphType
)");
}
