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

#include "spatial_graph_utilities.hpp"
#include "spatial_graph.hpp"
#include "spatial_node.hpp"

namespace SG {
std::pair<std::vector<SpatialNode::PointType>, std::vector<graph_descriptor> >
get_all_points(const GraphType &graph) {

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
