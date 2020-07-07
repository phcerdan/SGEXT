/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include "spatial_graph_difference.hpp"

namespace py = pybind11;
using namespace SG;

void init_spatial_graph_difference(py::module &m) {
    m.def("spatial_graph_difference", &spatial_graph_difference,
            R"(
Compute the difference between graphs using their spatial location
Returns: D = M - S

The difference uses vertex positions. Edge positions are ignored.

Nodes with adjacent nodes in M are kept, even if that same node exists in S.

Parameters:
----------
minuend: GraphType
 M in D = M - S
substraend: GraphType
 S in D = M - S
radius: Float
 radius used to search for neighbors in the octree point locator
 constructed with the two input graphs.
verbose: Bool
 extra information to console

Returns the difference graph.
)",
            py::arg("minuend"),
            py::arg("substraend"),
            py::arg("radius"),
            py::arg("verbose") = false
            );
}
