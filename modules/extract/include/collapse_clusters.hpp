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
