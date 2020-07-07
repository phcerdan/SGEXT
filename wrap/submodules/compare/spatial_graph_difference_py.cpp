/* ********************************************************************
 * Copyright (C) 2020 Pablo Hernandez-Cerdan.
 *
 * This file is part of SGEXT: http://github.com/phcerdan/sgext.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * *******************************************************************/

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
