/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

#include "visualize_spatial_graph.hpp"
#include "get_vtk_points_from_graph.hpp"
#include "visualize_spatial_graph_with_image.hpp"
#include "scripts_types.hpp" // For BinaryImageType

namespace py = pybind11;
using namespace SG;
void init_visualize_spatial_graph(py::module &m) {

/* ************************************************** */

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

/* ************************************************** */

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

/* ************************************************** */

    m.def("view_spatial_graph_with_binary_image",
    [](
    const GraphType & graph,
    const BinaryImageType::Pointer & image,
    const std::string & win_title,
    size_t & win_x,
    size_t & win_y
    ) {
    return visualize_spatial_graph_with_image<BinaryImageType>(
            graph, image, win_title, win_x, win_y);
    },
R"delimiter(
Visualize the spatial graph along a binary image. The binary image can be the original
binary image before the thinning.

Parameters:
----------
graph: GraphType
    Spatial graph to visualize

image: BinaryImageType
    input binary image

win_title: str
    title of the vtk window

winX,Y: int
    length of the side (X, Y) of the window.
)delimiter",
            py::arg("graph"),
            py::arg("image"),
            py::arg("win_title") = "sgext: SpatialGraph and Image",
            py::arg("wix_x") = 600,
            py::arg("win_y") = 600
            );

}
