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
