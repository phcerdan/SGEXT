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

#include "create_vertex_to_radius_map.hpp"

namespace py = pybind11;
using namespace SG;

void init_create_vertex_to_radius_map(py::module &m) {

    m.def("create_vertex_to_radius_map", [](
          const typename FloatImageType::Pointer & distance_map_image,
          const GraphType & input_graph,
          const bool spatial_nodes_position_are_in_physical_space,
          const bool verbose
          ){
            return create_vertex_to_radius_map(
                distance_map_image,
                input_graph,
                spatial_nodes_position_are_in_physical_space,
                verbose);
          },
R"(Create a vertex to local radius map from a distance map and a graph.

It can be used in tree_generation.

Parameters:
----------
distance_map_image: FloatImageType
  sgext image obtained from a binary image.
  See sgext.scripts.create_distance_map_io to obtain the distance map image.
graph: GraphType
  input spatial graph to get the vertices/nodes
spatial_nodes_position_are_in_physical_space: Bool [False]
  If False, the position of the spatial points in the graph are in index space.
  If True, they were converted to physical space via
  sgext.itk.transform_graph_to_physical_point
verbose: Bool [False]
  extra information at execution
)",
          py::arg("distance_map_image"), py::arg("graph"),
          py::arg("spatial_nodes_position_are_in_physical_space") = false,
          py::arg("verbose") = false);
}

