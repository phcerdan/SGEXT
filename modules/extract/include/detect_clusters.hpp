/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SGEXT_DETECT_CLUSTERS_HPP
#define SGEXT_DETECT_CLUSTERS_HPP

#include "spatial_graph.hpp"

namespace SG {

/**
 * Use detect_clusters_visitor to obtain a cluster label map, mappings
 * each vertex that belongs to a cluster to a label.
 *
 * For assigning that cluster label to spatial_node_id use @ref
 * assign_cluster_label_to_spatial_node_id
 *
 *
 * @param input_sg graph from where detect clusters
 * @param cluster_radius the cluster condition
 * @param verbose
 *
 * @return  cluster label map
 */
std::unordered_map<GraphType::vertex_descriptor, size_t>
detect_clusters_with_radius(const GraphType &input_sg,
                            const double &cluster_radius,
                            bool verbose = false);

/**
 * Modify spatial_node::id of input_sg with the cluster label.
 * Only modifies spatial_node::id that belong to a cluster.
 *
 * Useful for visualization.
 *
 * @param input_sg  input spatial graph
 * @param cluster_label_map obtained from @ref detect_clusters_with_radius or
 * similar.
 */
void assign_cluster_label_to_spatial_node_id(
        GraphType &input_sg,
        std::unordered_map<GraphType::vertex_descriptor, size_t>
                &cluster_label_map);
} // end namespace SG
#endif
