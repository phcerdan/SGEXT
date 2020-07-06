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
#include "collapse_clusters.hpp"

namespace py = pybind11;
using namespace SG;

void init_collapse_clusters(py::module &m) {
    m.def("collapse_clusters", &collapse_clusters,
          R"(
Collapse all the clusters specified in the vertex_to_single_label_cluster_map
of the spatial graph, generating a new graph where each cluster is
represented by one node.
The internal-cluster nodes and edges are removed, but the out-edges are kept.
Where they might be pointing to collapsed nodes, if they where connected to
other cluster.

This function needs to first run detect_clusters to obtain the
vertex_to_single_label_cluster_map

Parameters:
----------
graph: GraphType
 input spatial graph
vertex_to_cluster_label_map: Dict(Int -> Int (cluster label))
 obtained from detect_clusters, map each vertex that belongs to a cluster to
 a label.
verbose: Bool
 Print extra info to console.
)",
          py::arg("graph"),
          py::arg("vertex_to_cluster_label_map"),
          py::arg("verbose") = false);

/* ******************************************************************/

    m.def("trim_vertex_to_single_label_map", &trim_vertex_to_single_label_map,
          R"(
Returns a new vertex_single_label_cluster where only the vertex with labels
in the input cluster_labels are kept.

Parameters:
----------
cluster_labels: [int]
 list of vertex descriptors to keep
vertex_to_cluster_label_map: Dict(Int -> Int (cluster label))
 obtained from detect_clusters, map each vertex that belongs to a cluster to
 a label.
)",
          py::arg("cluster_labels"),
          py::arg("vertex_to_cluster_label_map")
          );
/* ******************************************************************/

    m.def("collapse_specific_clusters", &collapse_specific_clusters,
          R"(
Helper high-level function combining @ref trim_vertex_to_single_label_map and
@ref collapse_clusters Returns a copy of the graph after collapsing only the
clusters specified in cluster_labels.

@param cluster_labels cluster labels to collapse
@param input_sg input spatial graph
@param vertex_to_single_label_cluster_map vertex label map obtained
from detect_clusters functions
@param verbose

Parameters:
----------
cluster_labels: [int]
 list of vertex descriptors to collapse
graph: GraphType
 input spatial graph
vertex_to_cluster_label_map: Dict(Int -> Int (cluster label))
 obtained from detect_clusters, map each vertex that belongs to a cluster to
 a label.
verbose: Bool
 Print extra info to console.
)",
          py::arg("graph"),
          py::arg("cluster_labels"),
          py::arg("vertex_to_cluster_label_map"),
          py::arg("verbose") = false);
}
