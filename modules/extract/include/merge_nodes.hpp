/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MERGE_NODES_HPP
#define MERGE_NODES_HPP

#include "spatial_graph.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <iostream>
#include <tuple>

namespace SG {

/**
 * Some nodes that are connected between them could be merged.
 * Transforming three nodes with degree 3, to one node with degree 3
 * and adding the old nodes into the corresponding spatial edges.
 *
 *    o                o
 *    |                |
 *    o                |
 *    |\___            |
 *    |    \           |
 *    o-----o---o  ->  o---------o
 *    |                |
 *    |                |
 *    o                o
 *
 * Note: if two of those nodes are connected between them with an
 * extra parallel edge, the merge doesn't happen, example:
 *
 *        (a)
 *         o--
 *   (b) o/|  |
 *        \o--
 *        (c)
 *
 * See related tests for further details.
 *
 * @param sg input spatial graph to reduce.
 *
 * @return number of nodes merged/cleared.
 */
size_t merge_three_connected_nodes(GraphType &sg, bool inPlace = true);
// TODO: refactor/merge into merge_three_connected_nodes
size_t merge_four_connected_nodes(GraphType &sg, bool inPlace = true);
size_t merge_two_three_connected_nodes(GraphType &sg, bool inPlace = true);

std::vector<std::pair<boost::graph_traits<GraphType>::edge_descriptor,
                      boost::graph_traits<GraphType>::edge_descriptor> >
get_parallel_edges(const GraphType &sg);

std::vector<std::pair<boost::graph_traits<GraphType>::edge_descriptor,
                      boost::graph_traits<GraphType>::edge_descriptor> >
get_equal_parallel_edges(
        const std::vector<
                std::pair<boost::graph_traits<GraphType>::edge_descriptor,
                          boost::graph_traits<GraphType>::edge_descriptor> >
                &parallel_edges,
        const GraphType &sg);

} // namespace SG

#endif
