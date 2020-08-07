/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

#include "get_vtk_points_from_graph.hpp"
#include "scripts_types.hpp" // For BinaryImageType
#include "visualize_spatial_graph.hpp"
#include "visualize_spatial_graph_with_image.hpp"

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

win_title: str
    Title of the visualizing window

win_x,y: int
    length of the side (x, y) of the window.
)delimiter",
          py::arg("input"),
          py::arg("win_title") = SG::defaults::graph_layout_win_title,
          py::arg("win_x") = SG::defaults::graph_layout_win_width,
          py::arg("win_y") = SG::defaults::graph_layout_win_height);

    /* ************************************************** */

    m.def(
            "view_spatial_graph_with_points",
            [](const GraphType &graph, const double &opacity,
               const std::array<double, 3> cube_length,
               const std::string &winTitle, const size_t &win_x,
               const size_t &win_y) {
                const auto points_map_pair = get_vtk_points_from_graph(graph);
                visualize_spatial_graph_with_points(
                        graph, points_map_pair.first, opacity, cube_length,
                        winTitle, win_x, win_y);
            },
            R"delimiter(
Visualize the spatial graph with all the edge points.

Parameters:
----------
input: GraphType
    Spatial graph to visualize

opacity: float
    opacity of the points

cube_length: 3D Array (list)
    3D array with cube length (X, Y and Z) of the hyper-rectangle representing a point.

win_title: str
    Title of the visualizing window

win_x,y: int
    length of the side (x, y) of the window.
)delimiter",
            py::arg("input"), py::arg("opacity") = 0.8,
            py::arg("cube_length") = std::array<double, 3>({{1.0, 1.0, 1.0}}),
            py::arg("win_title") = SG::defaults::graph_layout_win_title,
            py::arg("win_x") = SG::defaults::graph_layout_win_width,
            py::arg("win_y") = SG::defaults::graph_layout_win_height);

    /* ************************************************** */

    m.def(
            "view_spatial_graph_with_binary_image",
            [](const GraphType &graph, const BinaryImageType::Pointer &image,
               const bool with_edge_points,
               const std::array<double, 3> cube_length,
               const std::string &win_title, size_t &win_x, size_t &win_y) {
                return visualize_spatial_graph_with_image<BinaryImageType>(
                        graph, image, with_edge_points, cube_length, win_title,
                        win_x, win_y);
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

with_edge_points: bool
    draw the edge points of the graph edges

cube_length: 3D Array (list)
    3D array with the dimensions of the hypercube representing the edge_points
    (only used when with_edge_points = true

win_title: str
    title of the vtk window

win_x,y: int
    length of the side (x, y) of the window.
)delimiter",
            py::arg("graph"), py::arg("image"),
            py::arg("with_edge_points") = false,
            py::arg("cube_length") = std::array<double, 3>({{1.0, 1.0, 1.0}}),
            py::arg("win_title") = "sgext: SpatialGraph and Image",
            py::arg("win_x") = 600, py::arg("win_y") = 600);

    /* ************************************************** */

    m.def(
            "view_poly_data",
            [](vtkSmartPointer<vtkPolyData> &poly_data,
               vtkSmartPointer<vtkLookupTable> &lut,
               const std::string &win_title, size_t &win_x, size_t &win_y) {
                return visualize_poly_data(poly_data, lut, win_title, win_x,
                                           win_y);
            },
            R"delimiter(
Visualize the spatial graph along a binary image. The binary image can be the original
binary image before the thinning.

Parameters:
----------
poly_data: vtkPolyData
    PolyData to visualize, obtained from reconstruct methods

lut: vtkLookupTable
     lookup table for colors

win_title: str
    title of the vtk window

win_x,y: int
    length of the side (x, y) of the window.
)delimiter",
            py::arg("poly_data"), py::arg("lut") = nullptr,
            py::arg("win_title") = "sgext: view_poly_data ",
            py::arg("win_x") = 600, py::arg("win_y") = 600);
}
