/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SPLIT_EDGE_HPP
#define SPLIT_EDGE_HPP

#include "spatial_edge.hpp"
#include "spatial_graph.hpp"
#include "spatial_node.hpp"

namespace SG {

template <typename SpatialGraphType> struct SplitEdge {
    bool point_exist_in_edge;
    size_t edge_point_index;
    typename SpatialGraphType::vertex_descriptor vertex_descriptor_added;
    std::vector<typename SpatialGraphType::edge_descriptor>
            edge_descriptors_added;
};

/**
 * Given a position or an index, a edge_descriptor, and a graph.
 * This functions splits the edge into two new edges and a new vertex at
 * the split location.
 * The old edge is removed.
 *
 * A--------B
 * becomes:
 * A---C----B
 *
 * @tparam SpatialGraphType spatial graph type
 * @param pos position
 * @param ed edge_descriptor
 * @param graph input/output graph (modified in place)
 *
 * @return SplitEdge struct with information about the new vertex and edges.
 */
template <typename SpatialGraphType>
SplitEdge<SpatialGraphType>
split_edge(const PointType &pos,
           const typename SpatialGraphType::edge_descriptor &ed,
           SpatialGraphType &graph) {
    const auto &ep = graph[ed].edge_points;
    auto found_it = std::find(std::begin(ep), std::end(ep), pos);
    if (found_it != std::end(ep)) {
        auto index_ep = std::distance(std::begin(ep), found_it);
        return split_edge(index_ep, ed, graph);
    } else {
        SplitEdge<SpatialGraphType> splitEdge;
        splitEdge.point_exist_in_edge = false;
        return splitEdge;
    }
}

template <typename SpatialGraphType>
SplitEdge<SpatialGraphType>
split_edge(const size_t edge_point_index,
           const typename SpatialGraphType::edge_descriptor &ed,
           SpatialGraphType &graph) {
    auto &ep = graph[ed].edge_points;
    if (ep.empty()) {
        std::runtime_error("Cannot split edge when edge_points of input "
                           "edge_descriptor is empty");
    }
    const auto ep_size = ep.size();
    if (edge_point_index >= ep_size) {
        std::runtime_error("edge_point_index (" +
                           std::to_string(edge_point_index) +
                           ") is too large for the input edge_descriptor");
    }
    SplitEdge<SpatialGraphType> splitEdge;
    splitEdge.point_exist_in_edge = true;
    splitEdge.edge_point_index = edge_point_index;

    SpatialNode sn;
    sn.pos = ep[edge_point_index];
    splitEdge.vertex_descriptor_added = boost::add_vertex(sn, graph);

    auto source = boost::source(ed, graph);
    auto target = boost::target(ed, graph);
    typename boost::edge_bundle_type<SpatialGraphType>::type spatialEdge_0;
    typename boost::edge_bundle_type<SpatialGraphType>::type spatialEdge_1;
    if (ep_size > 0) {
        auto &edge_points_0 = spatialEdge_0.edge_points;
        auto &edge_points_1 = spatialEdge_1.edge_points;
        // Note: iterators works with open ranges [a, b)
        // position associated to edge_points_index is not included in
        // edge_points_0:
        edge_points_0 = std::vector<PointType>(
                std::begin(ep), std::begin(ep) + edge_point_index);
        // The edge points do not include the position of the vertices so we add
        // 1 to edge_point_index
        edge_points_1 = std::vector<PointType>(
                std::begin(ep) + edge_point_index + 1, std::end(ep));
        bool source_is_closer_to_begin =
                ArrayUtilities::distance(graph[source].pos, ep[0]) <
                ArrayUtilities::distance(graph[target].pos, ep[0]);
        if (!source_is_closer_to_begin) {
            std::swap(source, target);
        }
    }

    splitEdge.edge_descriptors_added.reserve(2);
    auto edge_pair0 = boost::add_edge(source, splitEdge.vertex_descriptor_added,
                                      spatialEdge_0, graph);
    auto edge_pair1 = boost::add_edge(target, splitEdge.vertex_descriptor_added,
                                      spatialEdge_1, graph);
    splitEdge.edge_descriptors_added.push_back(edge_pair0.first);
    splitEdge.edge_descriptors_added.push_back(edge_pair1.first);

    boost::remove_edge(ed, graph);
    return splitEdge;
}
} // namespace SG
#endif
