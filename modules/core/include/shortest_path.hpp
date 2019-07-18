/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
std::vector<GraphType::vertex_descriptor>
compute_shortest_path(GraphType::vertex_descriptor start_vertex,
                      GraphType::vertex_descriptor end_vertex,
                      const GraphType &input_g,
                      bool verbose = false);

SpatialEdge create_edge_from_path(
        const std::vector<GraphType::vertex_descriptor> &vertex_path,
        const GraphType &input_g);

/**
 * Abort when reaching destination
 * From @sehe (stack-overflow): https://bit.ly/2Fp8o8o
 */
struct shortest_path_visitor : boost::default_dijkstra_visitor {
    using base = boost::default_dijkstra_visitor;
    using vertex_descriptor = SG::GraphType::vertex_descriptor;
    using edge_descriptor = SG::GraphType::edge_descriptor;
    struct done {};

    shortest_path_visitor(vertex_descriptor vd, size_t &visited)
            : destination(vd), visited(visited) {}

    inline void finish_vertex(vertex_descriptor v,
                              SG::GraphType const &input_sg) {
        ++visited;

        if (v == destination)
            throw done{};

        base::finish_vertex(v, input_sg);
    }

  private:
    vertex_descriptor destination;
    size_t &visited;
};

} // end namespace SG

#endif
