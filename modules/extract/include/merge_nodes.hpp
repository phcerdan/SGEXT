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

#ifndef MERGE_NODES_HPP
#define MERGE_NODES_HPP

#include <iostream>
#include <tuple>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include "spatial_graph.hpp"

namespace SG
{

/**
 * Some nodes that are connected between them could be merged.
 * Transforming three nodes with degree 3, to one node with degree 3
 * and adding the old nodes into the corresponding spatial edges.
 *
 * Note: if two of those nodes are connected between them with an
 * extra parallel edge, the merge doesn't happen, example:
 *
 *     (a)
 *      o--
 *(b) o/|  |
 *     \o--
 *     (c)
 *
 * See related tests for further details.
 *
 * @param sg input spatial graph to reduce.
 *
 * @return number of nodes merged/cleared.
 */
size_t merge_three_connected_nodes(GraphType & sg, bool inPlace = true);
// TODO: refactor/merge into merge_three_connected_nodes
size_t merge_four_connected_nodes(GraphType & sg, bool inPlace = true);
size_t merge_two_three_connected_nodes(GraphType & sg, bool inPlace = true);

std::vector< std::pair<boost::graph_traits< GraphType>::edge_descriptor,
                       boost::graph_traits< GraphType>::edge_descriptor > >
get_parallel_edges(const GraphType & sg);

std::vector<
std::pair<boost::graph_traits< GraphType>::edge_descriptor,
          boost::graph_traits< GraphType>::edge_descriptor > >
get_equal_parallel_edges(
    const std::vector<
    std::pair<boost::graph_traits< GraphType>::edge_descriptor,
              boost::graph_traits< GraphType>::edge_descriptor > >
    & parallel_edges, const GraphType & sg);

} //end namespace

#endif
