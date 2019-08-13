/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "spatial_graph_utilities.hpp"
#include "spatial_graph.hpp"
#include "spatial_node.hpp"

namespace SG {
std::pair<std::vector<SpatialNode::PointType>, std::vector<graph_descriptor> >
get_all_points(const GraphType &graph) {

    using vertex_descriptor = boost::graph_traits<GraphType>::vertex_descriptor;
    using vertex_iterator = boost::graph_traits<GraphType>::vertex_iterator;
    using edge_iterator = boost::graph_traits<GraphType>::edge_iterator;
    size_t num_points = num_edge_points(graph) + boost::num_vertices(graph);
    auto points = std::vector<SpatialNode::PointType>();
    points.reserve(num_points);
    auto gdescs = std::vector<graph_descriptor>();
    gdescs.reserve(num_points);

    vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(graph);
    for (; vi != vi_end; ++vi) {
        points.push_back(
                {graph[*vi].pos[0], graph[*vi].pos[1], graph[*vi].pos[2]});
        graph_descriptor gdesc;
        gdesc.exist = true;
        gdesc.is_vertex = true;
        gdesc.vertex_d = *vi;
        gdescs.push_back(gdesc);
    }

    edge_iterator ei, ei_end;
    std::tie(ei, ei_end) = boost::edges(graph);
    for (; ei != ei_end; ++ei) {
        auto &graph_edge = graph[*ei];
        auto &graph_edge_points = graph_edge.edge_points;
        for (size_t index = 0; index < graph_edge_points.size(); ++index) {
            const auto &p = graph_edge_points[index];
            points.push_back({p[0], p[1], p[2]});
            graph_descriptor gdesc;
            gdesc.exist = true;
            gdesc.is_edge = true;
            gdesc.edge_d = *ei;
            gdesc.edge_points_index = index;
            gdescs.push_back(gdesc);
        }
    }
#ifndef NDEBUG
    std::set<PointType> repeated_points;
    bool repeated_exists;
    std::tie(repeated_points, repeated_exists) =
            check_unique_points_in_graph(graph);
    if (repeated_exists) {
        std::cout << "Warning: duplicated points exist in "
                     "get_vtk_points_from_graph."
                     "Repeated Points: "
                  << repeated_points.size() << std::endl;
        for (const auto &p : repeated_points) {
            print_pos(std::cout, p);
            std::cout << std::endl;
        }
    }
    // assert(duplicated_points_exist == false);
#endif

    return std::make_pair(points, gdescs);
}

void print_pos(std::ostream &out, const SG::SpatialNode::PointType &pos) {
    out.precision(100);
    out << "{";
    for (auto &p : pos) {
        if (p >= 0)
            out << " ";
        out << p << ",";
    }
    out << "}";
}

AdjacentVerticesPositions
get_adjacent_vertices_positions(const GraphType::vertex_descriptor target_node,
                                const GraphType &g) {
    AdjacentVerticesPositions adjacent_vertices_positions;
    const auto neighbours = boost::adjacent_vertices(target_node, g);
    for (auto vd : make_iterator_range(neighbours)) {
        adjacent_vertices_positions.neighbours_descriptors.push_back(vd);
        adjacent_vertices_positions.neighbours_positions.push_back(g[vd].pos);
    }
    return adjacent_vertices_positions;
};

} // namespace SG
