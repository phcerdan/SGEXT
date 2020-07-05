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

/**
 * Return a vector of pairs of edges that are parallel between them.
 * If return_unique_pairs is true the out_edges a->b and b->a are considered
 * equal (undirected graph), and the associated duplicated pair is removed.
 *
 * @param sg input spatial graph
 * @param return_unique_pairs remove duplicated pair.
 * Recommended set it to true for undirected graphs
 *
 * @return vector of pairs of parallel edges
 */
std::vector<std::pair<boost::graph_traits<GraphType>::edge_descriptor,
                      boost::graph_traits<GraphType>::edge_descriptor> >
get_parallel_edges(const GraphType &sg, const bool return_unique_pairs = true);

/**
 * Check if the parallel_edges obtained from @ref get_parallel_edges
 * are exactly the same. Containing the same edge_points.
 *
 * @param parallel_edges obtained from @ref get_parallel_edges
 * @param sg input spatial graph
 *
 * @return vector of pairs of parallel_edges that have the same spatial_edge.
 */
std::vector<std::pair<boost::graph_traits<GraphType>::edge_descriptor,
                      boost::graph_traits<GraphType>::edge_descriptor> >
get_equal_parallel_edges(
        const std::vector<
                std::pair<boost::graph_traits<GraphType>::edge_descriptor,
                          boost::graph_traits<GraphType>::edge_descriptor> >
                &parallel_edges,
        const GraphType &sg);

/**
 * Use @ref get_parallel_edges to remove the edges of the input graph.
 * Return a new copy of the graph.
 *
 * @param sg input spatial graph
 * @param keep_largest_spatial_edges keep the parallel edge with largest
 * contour length, if false, keep shorter parallel edges.
 *
 * @return new graph without parallel edges
 */
GraphType remove_parallel_edges(const GraphType &sg,
                const bool keep_larger_spatial_edges = false);

} // namespace SG

#endif
