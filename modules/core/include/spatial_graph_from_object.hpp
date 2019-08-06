/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SPATIAL_GRAPH_FROM_OBJECT_HPP
#define SPATIAL_GRAPH_FROM_OBJECT_HPP

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <iostream>
#include <tuple>

namespace SG {

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
SpatialGraph spatial_graph_from_object(const ObjectGraph &obj) {
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
        for (size_t i = 0; i < ObjectGraph::Point::dimension; ++i) {
            sg[*svi].pos[i] = (*ovi)[i];
        }
        // Add to map
        dmap[*ovi] = *svi;
    }
    ovi = ovi_start;
    svi = svi_start;
    for (; ovi != ovi_end; ++ovi, ++svi) {
        // Add edges before checking they don't already exist (undirected graph)
        auto adjacents = boost::adjacent_vertices(*ovi, obj);
        for (; adjacents.first != adjacents.second; ++adjacents.first) {
            auto &s_adjacent = dmap[*adjacents.first];
            auto edge_exist = boost::edge(*svi, s_adjacent, sg);
            if (!edge_exist.second)
                boost::add_edge(*svi, dmap[*adjacents.first], sg);
        }
    }
    return sg;
}

} // namespace SG

#endif
