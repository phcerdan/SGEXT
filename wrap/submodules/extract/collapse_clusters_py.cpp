/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
