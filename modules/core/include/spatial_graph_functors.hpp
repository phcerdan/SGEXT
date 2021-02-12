/* ********************************************************************
 * Copyright (C) 2020 Pablo Hernandez-Cerdan.
 *
 * This file is part of SGEXT: http://github.com/phcerdan/sgext.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * *******************************************************************/

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
}

template <typename TFuncPos>
void operate_in_edge_points_pos(SpatialEdge &edge, TFuncPos func) {
    for (auto &ep : edge.edge_points) {
        func(ep);
    }
}

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
}
} // namespace SG

#endif
