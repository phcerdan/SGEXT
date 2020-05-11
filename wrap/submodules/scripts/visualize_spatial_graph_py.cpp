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

#include "visualize_spatial_graph.hpp"
#include "get_vtk_points_from_graph.hpp"

namespace py = pybind11;
using namespace SG;
void init_visualize_spatial_graph(py::module &m) {

    m.def("view_spatial_graph", &visualize_spatial_graph,
R"delimiter(
Visualize the spatial graph

Parameters:
----------
input: GraphType
    Spatial graph to visualize
)delimiter",
            py::arg("input")
            );

    m.def("view_spatial_graph_with_points",
    [](
    const GraphType & graph,
    const double & opacity,
    const double & lengthX,
    const double & lengthY,
    const double & lengthZ
    ) {
    const auto points_map_pair = get_vtk_points_from_graph(graph);
    visualize_spatial_graph_with_points(
        graph, points_map_pair.first,
        opacity, lengthX, lengthY, lengthZ);
    },
R"delimiter(
Visualize the spatial graph with all the edge points.

TODO: WARNING: Buggy, the window does not properly close in ipython.
Check: https://gitlab.kitware.com/vtk/vtk/-/issues/17251#note_754293
for any future update on how to solve it.

Parameters:
----------
input: GraphType
    Spatial graph to visualize

opacity: float
    opacity of the points

lengthX,Y,Z: float
    length of the side (X, Y or Z) of the hyper-rectangle representing a point.
)delimiter",
            py::arg("input"),
            py::arg("opacity") = 0.8,
            py::arg("lengthX") = 1.0,
            py::arg("lengthY") = 1.0,
            py::arg("lengthZ") = 1.0
            );

}
