/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "remove_extra_edges.hpp"
#include <tuple>

namespace SG {

bool remove_extra_edges(GraphType & sg) {
    bool any_edge_was_removed = false;
    using vertex_descriptor =  boost::graph_traits<
        GraphType>::vertex_descriptor;
    using vertex_iterator =
         boost::graph_traits<GraphType>::vertex_iterator;
    using adjacency_iterator =
         boost::graph_traits<GraphType>::adjacency_iterator;
    vertex_iterator vi, vi_end;
    std::vector<std::pair<vertex_descriptor, vertex_descriptor>> edges_to_remove;
    std::tie(vi, vi_end) = boost::vertices(sg);
    for (; vi != vi_end; ++vi) {
        auto degree = boost::out_degree(*vi, sg);
        if (degree > 2){
            adjacency_iterator neighbor_it, neighbor_end_it;
            std::tie(neighbor_it, neighbor_end_it) = boost::adjacent_vertices(*vi, sg);
            /* Compare distance between current node and neighbors
             * only if the neighbors are connected between them.
             * Remove the largest edge.
             * o
             * |\
             * oo
             */
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
                auto dist_between = ArrayUtilities::distance(sn_first.pos, sn_second.pos);
                if(dist_first > dist_second && dist_first > dist_between)
                    edges_to_remove.emplace_back(nv.first, *vi);
                else if(dist_second > dist_first && dist_second > dist_between)
                    edges_to_remove.emplace_back(nv.second, *vi);
                else if(dist_between > dist_first && dist_between > dist_second)
                    edges_to_remove.emplace_back(nv.first, nv.second);
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
}// end namespace
