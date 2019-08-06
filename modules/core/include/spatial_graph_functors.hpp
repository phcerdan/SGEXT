/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SPATIAL_GRAPH_FUNCTORS_HPP
#define SPATIAL_GRAPH_FUNCTORS_HPP

#include "spatial_edge.hpp"
#include "spatial_graph.hpp"
#include "spatial_node.hpp"

namespace SG {
/**
 * Interface to apply a function (usually a lambda) to the graph components.
 * From SpatialNode, SpatialEdge or the whole GraphType.
 *
 * Usage example:
 *
 * \code{.cpp}
 *  auto func_plus = [&disturbance](SG::PointType & pos){
 *       pos = ArrayUtilities::plus(pos, disturbance);
 *  };
 *  SG::operate_in_graph_pos(moved_g0, func_plus);
 * \endcode
 *
 * @tparam TFuncPos Any function operating in SG::PointType (Array3D)
 * @param node
 * @param func
 */
template <typename TFuncPos>
void operate_in_node_pos(SpatialNode &node, TFuncPos func) {
    func(node.pos);
};

template <typename TFuncPos>
void operate_in_edge_points_pos(SpatialEdge &edge, TFuncPos func) {
    for (auto &ep : edge.edge_points) {
        func(ep);
    }
};

template <typename TFuncPos>
void operate_in_graph_pos(GraphType &sg, TFuncPos func) {
    auto verts = boost::vertices(sg);
    for (auto &&vi = verts.first; vi != verts.second; ++vi) {
        operate_in_node_pos(sg[*vi], func);
    }
    auto edges = boost::edges(sg);
    for (auto ei = edges.first; ei != edges.second; ++ei) {
        operate_in_edge_points_pos(sg[*ei], func);
    }
};
} // namespace SG

#endif
