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
 * Some nodes that are connected between them,
 * could be merged.
 * Transforming three nodes with degree 3, to one node with degree 3
 * and adding the old nodes into the corresponding spatial edge.
 *
 * Note: if two of those nodes are connected between them with an extra parallel edge
 *     (a)
 *      o--
 *(b) o/|  |
 *     \o--
 *     (c)
 *
 * The merge of those nodes doesn't happen.
 *
 * See related tests for further details.
 *
 * @param sg input spatial graph to reduce.
 *
 * @return number of nodes merged/cleared.
 */
size_t merge_three_connected_nodes(GraphType & sg);

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
