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

#ifndef SHORTEST_PATH_HPP
#define SHORTEST_PATH_HPP
#include "spatial_graph.hpp"
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <vector>
namespace SG {
/**
 *  Compute the shortest path between start and end vertices.
 *
 *  As weight, it uses the contour length (i.e. all the edge points) of
 *  the edges.
 *
 * @param start_vertex start the search here
 * @param end_vertex stop the search here
 * @param input_g input spatial graph
 * @param verbose flag to output information to std::cout
 *
 * @return all the vertex descriptors involved in the path, from start to end.
 */
std::vector<GraphType::vertex_descriptor> compute_shortest_path(
    GraphType::vertex_descriptor start_vertex,
    GraphType::vertex_descriptor end_vertex,
    const GraphType& input_g,
    bool verbose = false);

SpatialEdge create_edge_from_path(
    const std::vector<GraphType::vertex_descriptor> & vertex_path,
    const GraphType & input_g);

/**
 * Abort when reaching destination
 * From @sehe (stack-overflow): https://bit.ly/2Fp8o8o
 */
struct shortest_path_visitor : boost::default_dijkstra_visitor {
  using base = boost::default_dijkstra_visitor;
  using vertex_descriptor = SG::GraphType::vertex_descriptor;
  using edge_descriptor = SG::GraphType::edge_descriptor;
  struct done {};

  shortest_path_visitor(vertex_descriptor vd, size_t& visited)
      : destination(vd), visited(visited) {}

  inline void finish_vertex(vertex_descriptor v, SG::GraphType const& input_sg) {
    ++visited;

    if(v == destination) throw done{};

    base::finish_vertex(v, input_sg);
  }

private:
  vertex_descriptor destination;
  size_t& visited;
};

} // end namespace SG

#endif
