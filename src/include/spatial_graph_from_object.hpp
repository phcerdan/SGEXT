#ifndef SPATIAL_GRAPH_FROM_OBJECT_HPP
#define SPATIAL_GRAPH_FROM_OBJECT_HPP

#include <iostream>
#include <tuple>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

namespace SG
{

/**
 * Convert (copy) object to spatial graph.
 *
 * Object has no stored edges, use its topology to list connected edges.
 * So we cannot remove edges without modifying the topology.
 *
 * The vertex_descriptor of object (the position) is stores in the nodes of
 * spatial graph: a_sg_node.pos
 * The sg_edges are not populated (there are no edge_points).
 *
 * @tparam ObjectGraph Object from DGtal
 * @tparam SpatialGraph
 * @param obj
 *
 * @return SpatialGraph which is a copy of object.
 */
template <class ObjectGraph, class SpatialGraph>
SpatialGraph spatial_graph_from_object(const ObjectGraph & obj)
{
    SpatialGraph sg(boost::num_vertices(obj));
    using obj_vertex_iterator =
        typename boost::graph_traits<ObjectGraph>::vertex_iterator;
    using obj_vertex_descriptor =
        typename boost::graph_traits<ObjectGraph>::vertex_descriptor;
    using sg_vertex_iterator =
        typename boost::graph_traits<SpatialGraph>::vertex_iterator;
    using sg_vertex_descriptor =
        typename boost::graph_traits<SpatialGraph>::vertex_descriptor;
    obj_vertex_iterator ovi, ovi_end, ovi_start;
    std::tie(ovi, ovi_end) = boost::vertices(obj);
    ovi_start = ovi;
    sg_vertex_iterator svi, svi_end, svi_start;
    std::tie(svi, svi_end) = boost::vertices(sg);
    svi_start = svi;
    std::unordered_map<obj_vertex_descriptor, sg_vertex_descriptor> dmap;
    for (; ovi != ovi_end; ++ovi, ++svi) {
        // Copy node position
        for (size_t i = 0; i < ObjectGraph::Point::dimension; ++i)
            sg[*svi].pos[i] = (*ovi)[i];
        // Add to map
        dmap[*ovi] = *svi;
    }
    ovi = ovi_start;
    svi = svi_start;
    for (; ovi != ovi_end; ++ovi, ++svi) {
        // Add edges before checking they don't already exist (undirected graph)
        auto adjacents = boost::adjacent_vertices(*ovi, obj);
        for (; adjacents.first != adjacents.second; ++adjacents.first) {
            auto & s_adjacent = dmap[*adjacents.first];
            auto edge_exist = boost::edge(*svi, s_adjacent, sg);
            if(!edge_exist.second)
                boost::add_edge(*svi, dmap[*adjacents.first], sg);
        }
    }
    return sg;
}

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
 */
template <class SpatialGraph>
void remove_extra_edges(SpatialGraph & sg) {
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
        if (edge_exist.second)
          boost::remove_edge(edge_exist.first, sg);
    }
}

} //end namespace

#endif
