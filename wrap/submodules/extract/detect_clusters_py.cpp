/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include "detect_clusters.hpp"

namespace py = pybind11;
using namespace SG;

void init_detect_clusters(py::module &m) {
    m.def("detect_clusters_with_radius", &detect_clusters_with_radius,
          R"(
Obtain a vertex to cluster label map, mapping each vertex that belongs to
a cluster to a label.

Returns a vertex_to_cluster_label_map, that can be used to collapse_clusters.

Parameters:
----------
graph: GraphType
 input spatial graph
radius: Float
 cluster radius, all nodes close to other nodes (at a distance = radius)
 form a cluster
use_cluster_centroid: Bool
 the node representing the whole cluster is the one closer to the
 cluster centroid.
 If False, the node is the one with the smalled vertex_descriptor.
verbose: Bool
 Print extra info to console.

)",
          py::arg("graph"),
          py::arg("radius"),
          py::arg("use_cluster_centroid") = true,
          py::arg("verbose") = false);

/* *********************************************************************/

    m.def("assign_label_to_spatial_node_id", &assign_label_to_spatial_node_id,
            R"(
Assign to spatial_node::id of each node of input_sg with the associated label
from vertex_to_label_map. Only modifies those spatial_node::id that exist in
the input vertex_to_label_map.

Useful for visualization.

Parameters:
----------

graph: GraphType
  input spatial graph
vertex_to_label_map: dict[int->int]
  vertex to label map. Obtained from sgext.detect_clusters_with_radius
  or similar methods that generation vertex to label maps.
)",
          py::arg("graph"),
          py::arg("vertex_to_label_map"));
}
