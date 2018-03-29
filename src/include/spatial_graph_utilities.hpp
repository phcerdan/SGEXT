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

#ifndef SPATIAL_GRAPH_UTILITIES_HPP
#define SPATIAL_GRAPH_UTILITIES_HPP

#include "spatial_graph.hpp"
#include "spatial_node.hpp"

namespace SG {

inline void print_pos(std::ostream &out, const SG::SpatialNode::PointType &pos) {
    out << "{";
    for (auto &p : pos) {
        if (p >= 0)
            out << " ";
        out << p << ",";
    }
    out << "}";
}

inline void print_degrees(const GraphType &graph) {
    std::cout << "Print degrees spatial_graph:" << std::endl;
    std::cout << "Num Vertices: " << boost::num_vertices(graph) << std::endl;
    using vertex_iterator =
        boost::graph_traits<GraphType>::vertex_iterator;
    vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(graph);
    for (; vi != vi_end; ++vi) {
        std::cout << *vi << ": " << ArrayUtilities::to_string(graph[*vi].pos) <<
            ". Degree: " << boost::out_degree(*vi, graph) << std::endl;
    }
}

inline void print_edges(const GraphType &graph) {
    std::cout << "Print edges spatial_graph:" << std::endl;
    std::cout << "Num Edges: " << boost::num_edges(graph) << std::endl;
    using edge_iterator =
        boost::graph_traits<GraphType>::edge_iterator;
    edge_iterator ei, ei_end;
    std::tie(ei, ei_end) = boost::edges(graph);
    for (; ei != ei_end; ++ei) {
        auto source = boost::source(*ei, graph);
        auto target = boost::target(*ei, graph);
        print_pos(std::cout, graph[source].pos);
        std::cout << "---";
        print_pos(std::cout, graph[target].pos);
        std::cout << std::endl;
    }
}

inline void print_spatial_edges(const GraphType &graph) {
    std::cout << "Print edges spatial_graph:" << std::endl;
    std::cout << "Num Edges: " << boost::num_edges(graph) << std::endl;
    using edge_iterator =
        boost::graph_traits<GraphType>::edge_iterator;
    edge_iterator ei, ei_end;
    std::tie(ei, ei_end) = boost::edges(graph);
    for (; ei != ei_end; ++ei) {
        auto source = boost::source(*ei, graph);
        auto target = boost::target(*ei, graph);
        print_pos(std::cout, graph[source].pos);
        std::cout << "---";
        print_pos(std::cout, graph[target].pos);
        std::cout << std::endl;
        std::cout << "edge_points: " << graph[*ei].edge_points.size() << std::endl;
        std::cout << graph[*ei] << std::endl;
    }
}
} //end namespace
#endif
