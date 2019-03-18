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

}  // namespace SG
#endif
