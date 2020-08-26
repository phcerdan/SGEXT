/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SGEXT_DETECT_CLUSTERS_HPP
#define SGEXT_DETECT_CLUSTERS_HPP

#include "spatial_graph.hpp"

namespace SG {

/**
 * Use detect_clusters_visitor to obtain a cluster label map, mapping
 * each vertex that belongs to a cluster to a label.
 *
 * For assigning that cluster label to spatial_node_id (useful for visualization
 * purposes) use @ref assign_label_to_spatial_node_id
 *
 * @param input_sg graph from where detect clusters
 * @param cluster_radius the cluster condition
 * @param use_cluster_centroid the node representing the whole cluster
 *  is the one closer to the cluster centroid.  If false, the node is the one
 *  with the smalled vertex_descriptor.
 * @param verbose
 *
 * @return  cluster label map
 */
std::unordered_map<GraphType::vertex_descriptor, GraphType::vertex_descriptor>
detect_clusters_with_radius(const GraphType &input_sg,
                            const double &cluster_radius,
                            bool use_cluster_centroid = true,
                            bool verbose = false);

/**
 * Assign to spatial_node::id of each node of input_sg with the associated label
 * from vertex_to_label_map. Only modifies those spatial_node::id that exist in
 * the input vertex_to_label_map.
 *
 * Useful for visualization.
 *
 * @param input_sg  input spatial graph
 * @param vertex_to_label_map obtained from @ref detect_clusters_with_radius or
 * similar.
 */
void assign_label_to_spatial_node_id(
        GraphType &input_sg,
        const std::unordered_map<GraphType::vertex_descriptor,
                           GraphType::vertex_descriptor> &vertex_to_label_map);
} // end namespace SG
#endif
