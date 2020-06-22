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
