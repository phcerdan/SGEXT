/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SGEXT_COLLAPSE_CLUSTERS_HPP
#define SGEXT_COLLAPSE_CLUSTERS_HPP

#include "spatial_graph.hpp"

namespace SG {

/**
 * Collapse all the clusters of the spatial graph, generating a new graph
 * where each clusters is represented by one node.
 * The internal-cluster nodes and edges are removed, but the out-edges are kept.
 * Where they might be pointing to collapsed nodes, if they where connected to
 * other cluster.
 *
 * This function needs to first run detect_clusters to obtain the
 * cluster_label_map
 *
 * @param input_sg input spatial graph
 * @param cluster_label_map obtained from @ref detect_clusters_with_radius or
 * equivalent.
 *
 * @return
 */
GraphType collapse_clusters(const GraphType &input_sg,
                            std::unordered_map<GraphType::vertex_descriptor,
                                               GraphType::vertex_descriptor> &vertex_to_single_label_cluster_map,
                            bool verbose);
} // end namespace SG
#endif
