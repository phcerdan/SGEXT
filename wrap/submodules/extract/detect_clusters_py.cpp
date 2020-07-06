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
}
