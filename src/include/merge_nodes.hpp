#ifndef MERGE_NODES_HPP
#define MERGE_NODES_HPP

#include <iostream>
#include <tuple>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

namespace SG
{
/**
 * Some nodes that are connected between them,
 * could be merged.
 * Transorming 3 nodes with degree 3, to 1 node with degree 3
 * and adding the old nodes into the corresponding spatial edge.
 *
 * See related tests for further details.
 *
 * @param sg input spatial graph to reduce.
 *
 * @return number of nodes merged/cleared.
 */
template <class SpatialGraph>
size_t merge_three_connected_nodes(SpatialGraph & sg) {
    size_t node_was_merged = 0;
    using vertex_descriptor = typename boost::graph_traits<
        SpatialGraph>::vertex_descriptor;
    using vertex_iterator =
        typename boost::graph_traits<SpatialGraph>::vertex_iterator;
    using adjacency_iterator =
        typename boost::graph_traits<SpatialGraph>::adjacency_iterator;
    vertex_iterator vi, vi_end;
    using VertexPair = std::pair<vertex_descriptor, vertex_descriptor>;
    using VertexTriple = std::tuple<vertex_descriptor, vertex_descriptor, vertex_descriptor>;
    std::vector<VertexPair> nodes_to_remove;
    std::vector<VertexTriple> edges_to_remove;
    std::tie(vi, vi_end) = boost::vertices(sg);
    for (; vi != vi_end; ++vi) {
        auto degree_current = boost::out_degree(*vi, sg);
        if (degree_current > 2){
            // Check that current node is not already selected to remove:
            {
                auto already_selected_to_delete = std::find_if(
                        std::begin(nodes_to_remove),
                        std::end(nodes_to_remove),
                        [&vi](const VertexPair & p) {
                        return p.first == *vi;
                        }
                        );
                if(already_selected_to_delete != std::end(nodes_to_remove))
                    continue;
            }
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
                auto degree_first = boost::out_degree(nv.first, sg);
                auto degree_second = boost::out_degree(nv.second, sg);
                // If neighbors have more than degree 3, abort merge
                if (degree_first != 3 && degree_second != 3)
                    continue;

                auto edge_neighbors_pair = boost::edge(nv.first, nv.second, sg);
                auto edge_first_pair = boost::edge(*vi, nv.first, sg);
                auto edge_second_pair = boost::edge(*vi, nv.second, sg);
                auto & edge_neighbors = sg[edge_neighbors_pair.first];
                auto & edge_first = sg[edge_first_pair.first];
                auto & edge_second = sg[edge_second_pair.first];

                // If the edge_points are not empty, abort merge
                if (!(edge_neighbors.edge_points.empty() &&
                      edge_first.edge_points.empty() &&
                      edge_second.edge_points.empty()))
                    continue;

                nodes_to_remove.emplace_back(nv.first, *vi);
                nodes_to_remove.emplace_back(nv.second, *vi);
                edges_to_remove.emplace_back(nv.first, nv.second, *vi);
            }
        }
    }
    // First, remove edges between 3 connected nodes
    for(auto & edge_triple : edges_to_remove) {
        boost::remove_edge(
                std::get<0>(edge_triple),
                std::get<1>(edge_triple),
                sg);
        boost::remove_edge(
                std::get<0>(edge_triple),
                std::get<2>(edge_triple),
                sg);
        boost::remove_edge(
                std::get<1>(edge_triple),
                std::get<2>(edge_triple),
                sg);
    }

    // Connect new nodes to node_to_merge_into.
    for(auto & nodes : nodes_to_remove) {
        auto & node_to_remove = nodes.first;
        auto & node_to_merge_into = nodes.second;
        auto edges = boost::out_edges(node_to_remove, sg);
        // Create an edge to the node to merge into.
        for(auto ei = edges.first ; ei != edges.second; ++ei) {
            auto source = boost::source(*ei, sg);
            auto target = boost::target(*ei, sg);
            auto & spatial_edge = sg[*ei];
            // TODO, push_back might disorder the edge_points.
            // This might not be a problem, though.
            spatial_edge.edge_points.push_back(sg[node_to_remove].pos);
            boost::add_edge(node_to_merge_into, target, spatial_edge, sg);
            // boost::remove_edge(node_to_remove, target, sg);
        }
        // Remove vertex destroys all iterators/descripts
        // when using a vecS as vertex_intex_t
        // boost::remove_vertex(node_to_remove, sg);
        boost::clear_vertex(node_to_remove, sg);
        node_was_merged++;
    }
    return node_was_merged;
}
} //end namespace

#endif
