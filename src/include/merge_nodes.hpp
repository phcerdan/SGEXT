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
     * Insert point in the input container.
     * The input container is a list of points ordered by connectvity, consecutive points in the container are connected.
     *
     * This computes the distance of the new_point with all the existing edge_points.
     * TODO, an optimization would be to only compute it against first and last.
     *
     * PRECONDITION: edge_points are already ordered.
     *
     * @param edge_points container with existing points.
     * Ordered by connectivity, adjacent points are connected.
     * @param new_point point to intert.
     */
void insert_edge_point_with_distance_order(
        SG::SpatialEdge::PointContainer & edge_points,
        const SG::SpatialEdge::PointType & new_point);
/**
 * Some nodes that are connected between them,
 * could be merged.
 * Transforming 3 nodes with degree 3, to 1 node with degree 3
 * and adding the old nodes into the corresponding spatial edge.
 *
 * TODO:
 * Pitfall1: what happen when 2 of those nodes are connected between them with an extra parallel edge?
 *     (a)
 *      o--
 *(b) o/|  |
 *     \o--
 *     (c)
 *
 * Solution1: Don't do it in that case, (or use split_loop?)
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
