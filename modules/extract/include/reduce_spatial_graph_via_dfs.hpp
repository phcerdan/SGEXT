/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef REDUCE_SPATIAL_GRAPH_VIA_DFS_HPP
#define REDUCE_SPATIAL_GRAPH_VIA_DFS_HPP
#include "spatial_graph.hpp"

namespace SG {

/**
 * Uses ReduceGraphVisitor to create a new spatial graph from the input
 * with no chain-nodes (degree 2) and populated spatial edges with
 * the pos of those chain-nodes.
 *
 * The implementation right now starts the visit at end and
 * junction nodes (degree!=2) and then use SelfLoopVisitor
 * to visit unexplored nodes of degree == 2 (part of self-loops)
 * We do store the self-loops as 2 nodes of 2-degree, with 2 spatial edges using
 * @sa split_loop function.
 *
 *
 * @param input_sg input
 * @param verbose pass verbosity flag to follow the visit in std::cout
 *
 * @return reduced graph.
 */
GraphType reduce_spatial_graph_via_dfs(const GraphType &input_sg,
                                       bool verbose = false);

} // namespace SG
#endif
