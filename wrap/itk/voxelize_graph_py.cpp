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

#include "voxelize_graph.hpp"

namespace py = pybind11;
using namespace SG;

void init_voxelize_graph(py::module &m) {
    m.def("voxelize_graph", &voxelize_graph,
          R"delimiter(
Create an image from the input graph.
Uses a reference_image (for example, the thin image used to create the graph)
to set the size and metadata (origin, spacing, direction) of the output
image.

It takes two label maps, one for vertices and other for edges. You can
create an edge_to_label_map using the helper functions:
create_edge_to_label_map_from_vertex_to_label_map
create_edge_to_label_map_from_vertex_to_label_map_using_max

returns a BinaryImage

Parameters:
----------
graph: GraphType
 input spatial graph

reference_image: BinaryImageType
    binary image.

vertex_to_label_map: Dict[int -> int]
    Dict mapping vertices to label

edge_to_label_map: Dict[edge -> int]
    Dict mapping edges to label.
    You can create a edge_to_label_map from a vertex_to_label_map
    using create_edge_to_label_map_from_vertex_to_label_map family of functions.
    See create_edge_to_label_map_from_vertex_to_label_map_using_max for example.

graph_positions_are_in_physical_space: Bool
    Flag to check if the graph positions are in physical space.
    Graphs are in physical space after applying transform_graph_to_physical_space.
            )delimiter",
          py::arg("graph"), py::arg("reference_image"),
          py::arg("vertex_to_label_map"), py::arg("edge_to_label_map"),
          py::arg("graph_positions_are_in_physical_space") = true);

    /*********************************************************/

    const std::string graph_position_to_image_index_docs = R"(
Helper function to get an ITK image index from a graph position.

If the graph_positions_are_in_physical_space applies a
TransformPhysicalPointToIndex using the refence_image.

returns an itk_index

Parameters:
----------
input_point: Array3D
    input point from a spatial_node.pos or spatial_edge.edge_points[ep_index].
reference_image: BinaryImageType::Pointer or FloatImageType::Pointer
    reference ITK image
graph_positions_are_in_physical_space: Bool
    true if the positions are in physical space. See transform_graph_to_physical_space.
    )";

    m.def("graph_position_to_image_index",
          &graph_position_to_image_index<BinaryImageType>,
          graph_position_to_image_index_docs.c_str(), py::arg("input_point"),
          py::arg("reference_image"),
          py::arg("graph_position_to_image_index") = true);
    m.def("graph_position_to_image_index",
          &graph_position_to_image_index<FloatImageType>,
          graph_position_to_image_index_docs.c_str(), py::arg("input_point"),
          py::arg("reference_image"),
          py::arg("graph_position_to_image_index") = true);

    /*********************************************************/

    const std::string create_edge_to_label_map_from_vertex_to_label_map_docs =
            R"(
Create an edge_to_label_map (needed for @ref voxelize_graph) from a
vertex_to_label_map and a functor of type func(source_label, target_label) ->
edge_label.

If a existing node in the graph is not found in the input vertex_to_label_map
The functor won't be applied on the edges having that node as source or
target, and that edge won't be emplaced on the resulting edge map.

Parameters:
----------

graph: GraphType
    input spatial graph
vertex_to_label_map: Dict[int -> int]
    Dict mapping vertex to labels.
edge_label_func: func(int source_label, int target_label) -> int edge_label
    Functor that taking vertex labels: source and target, returns an edge label.
    )";
    m.def("create_edge_to_label_map_from_vertex_to_label_map",
          &create_edge_to_label_map_from_vertex_to_label_map,
          create_edge_to_label_map_from_vertex_to_label_map_docs.c_str(),
          py::arg("graph"), py::arg("vertex_to_label_map"),
          py::arg("edge_label_func"));

    /*********************************************************/

    const std::string
            create_edge_to_label_map_from_vertex_to_label_map_using_max_docs =
                    R"(

Use @ref create_edge_to_label_map_from_vertex_to_label_map with the functor
std::max(source_label, target_label).
Each edge will have the label corresponding to the max label from
source_label, target_label.

Parameters:
----------

graph: GraphType
    input spatial graph
vertex_to_label_map: Dict[int -> int]
    Dict mapping vertex to labels.
    )";
    m.def("create_edge_to_label_map_from_vertex_to_label_map_using_max",
          &create_edge_to_label_map_from_vertex_to_label_map_using_max,
          create_edge_to_label_map_from_vertex_to_label_map_using_max_docs
                  .c_str(),
          py::arg("graph"), py::arg("vertex_to_label_map"));
}
