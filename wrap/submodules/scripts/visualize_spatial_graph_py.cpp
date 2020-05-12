/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
