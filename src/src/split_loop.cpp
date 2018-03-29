/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "split_loop.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <algorithm>
#include <iostream>

namespace SG {

void split_loop(
    GraphType::vertex_descriptor loop_vertex_id,
    const boost::edge_bundle_type<GraphType>::type &sg_edge,
    GraphType &input_sg)
{
    using SpatialEdge = boost::edge_bundle_type<GraphType>::type;
    using SpatialNode = boost::vertex_bundle_type<GraphType>::type;
    auto &edge_points = sg_edge.edge_points;
    size_t middle_index = edge_points.size() / 2;
    // std::cout << "VertexId:" << loop_vertex_id << " .EdgePoints.size():" << edge_points.size()  <<". middle_index: " <<middle_index << std::endl;
    SpatialNode sg_node;
    sg_node.pos = edge_points[middle_index];
    auto created_vertex_id = boost::add_vertex(sg_node, input_sg);
    auto nth = std::next(edge_points.begin(), middle_index);
    SpatialEdge created_sg_edge1;
    created_sg_edge1.edge_points.insert(std::end(created_sg_edge1.edge_points),
                                        edge_points.begin(), nth);
    SpatialEdge created_sg_edge2;
    created_sg_edge2.edge_points.insert(std::end(created_sg_edge2.edge_points),
                                        std::next(nth), edge_points.end());
    boost::add_edge(loop_vertex_id, created_vertex_id,
        created_sg_edge1, input_sg);
    boost::add_edge(loop_vertex_id, created_vertex_id,
        created_sg_edge2, input_sg);
    // std::cout << "Degree Original: " << boost::degree(loop_vertex_id, input_sg) << std::endl;
    // std::cout << "Degree created: " << boost::degree(created_vertex_id, input_sg) << std::endl;
}

} // end namespace
