/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SGEXT_COLLAPSE_CLUSTERS_HPP
#define SGEXT_COLLAPSE_CLUSTERS_HPP

#include "spatial_graph.hpp"

namespace SG {

/**
 * Collapse all the clusters specified in the vertex_to_single_label_cluster_map
 * of the spatial graph, generating a new graph where each cluster is
 * represented by one node.
 * The internal-cluster nodes and edges are removed, but the out-edges are kept.
 * Where they might be pointing to collapsed nodes, if they where connected to
 * other cluster.
 *
 * This function needs to first run detect_clusters to obtain the
 * vertex_to_single_label_cluster_map
 *
 * @param input_sg input spatial graph
 * @param vertex_to_single_label_cluster_map obtained from
 *  @ref detect_clusters_with_radius or equivalent.
 * @param verbose extra info to cout
 *
 * @return
 */
GraphType
collapse_clusters(const GraphType &input_sg,
                  const std::unordered_map<GraphType::vertex_descriptor,
                                           GraphType::vertex_descriptor>
                          &vertex_to_single_label_cluster_map,
                  bool verbose = false);

/**
 * Returns a new vertex_single_label_cluster where only the vertex with labels
 * in the input cluster_labels are kept.
 *
 * For a easy to use collapse using this function see: @ref
 * collapse_specific_clusters
 *
 * @param cluster_labels vector of cluster labels to keep from the map
 * @param vertex_to_single_label_cluster_map vertex label map obtained
 * from detect_clusters functions
 *
 * @return a new vertex_single_label_cluster map containing only the input
 * labels
 */
std::unordered_map<GraphType::vertex_descriptor, GraphType::vertex_descriptor>
trim_vertex_to_single_label_map(
        const std::vector<GraphType::vertex_descriptor> &cluster_labels,
        const std::unordered_map<GraphType::vertex_descriptor,
                                 GraphType::vertex_descriptor>
                &vertex_to_single_label_cluster_map);

/**
 * Helper high-level function combining @ref trim_vertex_to_single_label_map and
 * @ref collapse_clusters Returns a copy of the graph after collapsing only the
 * clusters specified in cluster_labels.
 *
 * @param cluster_labels cluster labels to collapse
 * @param input_sg input spatial graph
 * @param vertex_to_single_label_cluster_map vertex label map obtained
 * from detect_clusters functions
 * @param verbose
 *
 * @return new spatial graph
 */
GraphType collapse_specific_clusters(
        const std::vector<GraphType::vertex_descriptor> &cluster_labels,
        const GraphType &input_sg,
        const std::unordered_map<GraphType::vertex_descriptor,
                                 GraphType::vertex_descriptor>
                &vertex_to_single_label_cluster_map,
        bool verbose = false);

} // end namespace SG
#endif
