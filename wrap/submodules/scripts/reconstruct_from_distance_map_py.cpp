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

#include "locate/sglocate_common.h" // define holder for vtkSmartPointer
#include "reconstruct_from_distance_map.hpp"

namespace py = pybind11;
using namespace SG;

void init_reconstruct_from_distance_map(py::module &m) {

    py::class_<vtkPolyData, vtkSmartPointer<vtkPolyData>>(m, "vtkPolyData")
            .def("__str__", [](vtkPolyData &p) {
                std::stringstream os;
                p.Print(os);
                return os.str();
            });
    py::class_<vtkLookupTable, vtkSmartPointer<vtkLookupTable>>(
            m, "vtkLookupTable")
            .def("__str__", [](vtkLookupTable &p) {
                std::stringstream os;
                p.Print(os);
                return os.str();
            });

    py::class_<ReconstructOutput>(m, "ReconstructOutput")
            .def_readwrite("poly_data", &ReconstructOutput::poly_data)
            .def_readwrite("lut", &ReconstructOutput::lut);

    m.def(
            "reconstruct_from_distance_map",
            [](const GraphType &input_sg,
               FloatImageType::Pointer &distance_map_image,
               const bool spatial_nodes_position_are_in_physical_space,
               const bool distance_map_image_use_image_spacing,
               const std::unordered_map<GraphType::vertex_descriptor, size_t>
                       &vertex_to_label_map,
               const bool apply_color_to_edges) {
                return reconstruct_from_distance_map(
                        input_sg, distance_map_image.GetPointer(),
                        spatial_nodes_position_are_in_physical_space,
                        distance_map_image_use_image_spacing,
                        vertex_to_label_map, apply_color_to_edges);
            },
            R"(
Create a mesh using all the points (including edge_points) from input graph
and the distance_map_image. The distance map can be in Voxels (DGtal), or
taking into account the image spacing using ITK (less precise) see
@ref create_distance_map.

Parameters:
---------
graph: GraphType
  input spatial graph to get the vertices/nodes
distance_map_image: FloatImageType
  distance map image
spatial_nodes_position_are_in_physical_space: Bool [False]
  node positions are in physical space (instead of default index space)
distance_map_image_use_image_spacing: Bool [False]
 the distance map values takes into account image spacing (false for DGtal,
 maybe true if computed via ITK) @ref create_distance_map
vertex_to_label_map map: Dict[int,int] [Empty]
 to assign different colors to the poly data, empty by default.
apply_color_to_edges: Bool [True]
 Edge points have a label associated to max(source_label,target_label) of that edge. Useful for generation analysis.
)",
            py::arg("graph"), py::arg("distance_map_image"),
            py::arg("spatial_nodes_position_are_in_physical_space") = false,
            py::arg("distance_map_image_use_image_spacing") = false,
            py::arg("vertex_to_label_map") =
                    std::unordered_map<GraphType::vertex_descriptor, size_t>(),
            py::arg("apply_color_to_edges") = true);

    /* ************************************************** */

    m.def(
            "view_poly_data",
            [](vtkSmartPointer<vtkPolyData> poly_data,
               vtkSmartPointer<vtkLookupTable> lut,
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

    /* ************************************************** */

    m.def(
            "write_poly_data",
            [](vtkSmartPointer<vtkPolyData> poly_data,
               const std::string &filename, const bool is_binary) {
                return write_poly_data(poly_data, filename, is_binary);
            },
            R"delimiter(
Write the polydata using XMLPolyDataWriter, filename needs to have suffix vtk, or vtu.

Parameters:
----------
poly_data: vtkPolyData
    PolyData to write, obtained from reconstruct methods

filename: str
    filename of the output. It should have the prefix vtk or vtu.

is_binary: Bool [False]
    if true: SetDataModeToBinary is applied
)delimiter",
            py::arg("poly_data"), py::arg("filename"),
            py::arg("is_binary") = true);

    /* ************************************************** */

    const std::string poly_data_to_binary_image_docs =
            R"delimiter(
Use vtkPolyDataToImageStencil to create an (ITK) binary image from input poly data.
It needs a reference_image to gather dimensions and metadata.

TODO WARNING: This functions is in development. The surface using vtkAppendPolyData
with spheres in reconstruct_from_distance_map produces internal holes
in the output image.

Parameters:
----------
poly_data: vtkPolyData
    PolyData, obtained from reconstruct methods

reference_image: BinaryImageType or FloatImageType
     reference image to get dimensions and metadata (spacing, origin, direction).
)delimiter";

    m.def(
            "poly_data_to_binary_image",
            [](vtkSmartPointer<vtkPolyData> poly_data,
               const BinaryImageType::Pointer &reference_image) {
                return poly_data_to_binary_image(poly_data, reference_image);
            },
            poly_data_to_binary_image_docs.c_str(), py::arg("poly_data"),
            py::arg("reference_image"));
    m.def(
            "poly_data_to_binary_image",
            [](vtkSmartPointer<vtkPolyData> poly_data,
               const FloatImageType::Pointer &reference_image) {
                return poly_data_to_binary_image(poly_data, reference_image);
            },
            poly_data_to_binary_image_docs.c_str(), py::arg("poly_data"),
            py::arg("reference_image"));
}
