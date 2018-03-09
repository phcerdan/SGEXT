#ifndef REMOVE_EXTRA_EDGES_HPP
#define REMOVE_EXTRA_EDGES_HPP

#include <iostream>
#include <tuple>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

namespace SG
{
/**
 * Object in DGtal with a 26_9 adjacency consider as adjacent vertices those in diagonals. We are interested in keeping that topology, but it generates spurious edges that confuses further analysis.
 * We remove the "diagonal" edges when there are shorter ones. Better an example:
 * o                  o
 * |\                 |
 * o-o      ----->    o-o
 * |/                 |
 * o                  o
 *
 * We are not interested in the diagonal edges, and better keep the center node as the only 3-degree node.
 *
 * See related tests for further details.
 *
 * @param sg input spatial graph to reduce.
 *
 * @return boolean, true if any edge has been removed
 * false otherwhise.
 */
template <class SpatialGraph>
bool remove_extra_edges(SpatialGraph & sg) {
    bool any_edge_was_removed = false;
    using vertex_descriptor = typename boost::graph_traits<
        SpatialGraph>::vertex_descriptor;
    using vertex_iterator =
        typename boost::graph_traits<SpatialGraph>::vertex_iterator;
    using adjacency_iterator =
        typename boost::graph_traits<SpatialGraph>::adjacency_iterator;
    vertex_iterator vi, vi_end;
    std::vector<std::pair<vertex_descriptor, vertex_descriptor>> edges_to_remove;
    std::tie(vi, vi_end) = boost::vertices(sg);
    for (; vi != vi_end; ++vi) {
        auto degree = boost::out_degree(*vi, sg);
        if (degree > 2){
            adjacency_iterator neighbor_it, neighbor_end_it;
            std::tie(neighbor_it, neighbor_end_it) = boost::adjacent_vertices(*vi, sg);
            // Compare distance between current node and neighbors
            // only if the neighbors are connected between them.
            // Remove the largest edge.
            // o
            // |\
            // oo
            std::vector<std::pair<vertex_descriptor, vertex_descriptor>> neighbors_connected;
            for(; neighbor_it != neighbor_end_it; ++neighbor_it) {
                auto other_neighbor_it = neighbor_it;
                ++other_neighbor_it;
                if (other_neighbor_it != neighbor_end_it){
                    for(; other_neighbor_it != neighbor_end_it; ++other_neighbor_it) {
                        auto edge_exist_pair = boost::edge(*neighbor_it, *other_neighbor_it, sg);
                        // Check edge between neighbors exist.
                        if(edge_exist_pair.second){
                            // Store pair of vertex_descriptor of connected neighbors
                            // Because removing edges causes invalidation of adjacency_iterator
                            neighbors_connected.emplace_back(*neighbor_it, *other_neighbor_it);
                        }
                    }
                }
            }

            // Compare positions of current and neighbor nodes,
            // and remove largest edge.
            for(auto & nv : neighbors_connected){
                auto & sn_first = sg[nv.first];
                auto & sn_second = sg[nv.second];
                auto & sn_current = sg[*vi];
                auto dist_first = ArrayUtilities::distance(sn_current.pos, sn_first.pos);
                auto dist_second = ArrayUtilities::distance(sn_current.pos, sn_second.pos);
                if(dist_first > dist_second) {
                    edges_to_remove.emplace_back(nv.first, *vi);
                } else if(dist_first < dist_second) {
                    edges_to_remove.emplace_back(nv.second, *vi);
                }
            }
        }
    }
    for(auto & edge : edges_to_remove) {
        auto edge_exist = boost::edge(edge.first, edge.second, sg);
        if (edge_exist.second){
          boost::remove_edge(edge_exist.first, sg);
          any_edge_was_removed = true;
        }
    }
    return any_edge_was_removed;
}
} //end namespace

#endif
