/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

#include "create_vertex_to_radius_map.hpp"

namespace py = pybind11;
using namespace SG;

void init_create_vertex_to_radius_map(py::module &m) {

    m.def("create_vertex_to_radius_map", &create_vertex_to_radius_map,
          R"(
Create a vertex to local radius map from a distance map and a graph.

It can be used in tree_generation.

Parameters:
---------
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

