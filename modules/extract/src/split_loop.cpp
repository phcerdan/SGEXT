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
