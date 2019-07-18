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

#ifndef SPLIT_LOOP_HPP
#define SPLIT_LOOP_HPP
#include "spatial_graph.hpp"

namespace SG {

/**
 * Split self-loop (node with an edge to itself) given the vertex_descriptor,
 * spatial_edge containing the edge_points, and the graph.
 * The graph will be modified adding an extra node in the position of the
 * median of the edge_points list.
 * The added node and the loop_vertex_id will be connected with two edges, each
 * containing half of the edge_points of the given sg_edge.
 *
 * Warning: it uses add_vertex, and add_edge so it can invalidate vertex and
 * edge descriptors, iterators etc if the graph uses a vector instead of a list
 * for storing vertices indices. See boost docs. For listS and vecS (undirected)
 * it only invalidates ALL adjacency_iterators.
 *
 * Also note that the adjacency_list must: allow_parallel_edge_tag. All
 * containers except setS and hash_setS allow it.
 * http://www.boost.org/doc/libs/1_60_0/libs/graph/doc/adjacency_list.html
 *
 * @param loop_vertex_id vertex_descriptor of the node that has edge connecting
 * to itself.
 * @param sg_edge SpatialEdge containing edge_points.
 * @param input_sg input graph
 *
 */
void split_loop(GraphType::vertex_descriptor loop_vertex_id,
                const boost::edge_bundle_type<GraphType>::type &sg_edge,
                GraphType &input_sg);

} // namespace SG
#endif
