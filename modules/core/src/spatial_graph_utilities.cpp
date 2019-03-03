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

size_t num_edge_points(const GraphType & sg)
{
    auto edges = boost::edges(sg);
    size_t num_points;
    for (auto ei = edges.first; ei != edges.second; ++ei) {
        for (auto & ep : sg[*ei].edge_points) {
            ++num_points;
        }
    }
    return num_points;
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

void print_degrees(const GraphType &graph) {
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

void print_edges(const GraphType &graph) {
    std::cout << "Print edges spatial_graph:" << std::endl;
    std::cout << "Num Edges: " << boost::num_edges(graph) << std::endl;
    using edge_iterator =
        boost::graph_traits<GraphType>::edge_iterator;
    edge_iterator ei, ei_end;
    std::tie(ei, ei_end) = boost::edges(graph);
    for (; ei != ei_end; ++ei) {
        auto source = boost::source(*ei, graph);
        auto target = boost::target(*ei, graph);
        std::cout << source << "---" << target << " ; " ;
        print_pos(std::cout, graph[source].pos);
        std::cout << "---";
        print_pos(std::cout, graph[target].pos);
        std::cout << std::endl;
    }
}

void print_spatial_edges(const GraphType &graph) {
    std::cout << "Print edges spatial_graph:" << std::endl;
    std::cout << "Num Edges: " << boost::num_edges(graph) << std::endl;
    using edge_iterator =
        boost::graph_traits<GraphType>::edge_iterator;
    edge_iterator ei, ei_end;
    std::tie(ei, ei_end) = boost::edges(graph);
    for (; ei != ei_end; ++ei) {
        auto source = boost::source(*ei, graph);
        auto target = boost::target(*ei, graph);
        std::cout << source << "---" << target << " ; " ;
        print_pos(std::cout, graph[source].pos);
        std::cout << "---";
        print_pos(std::cout, graph[target].pos);
        std::cout << std::endl;
        std::cout << "edge_points: " << graph[*ei].edge_points.size() << std::endl;
        std::cout << graph[*ei] << std::endl;
    }
}

std::pair<std::set<PointType>, bool> check_unique_points_in_graph(const GraphType & sg)
{
    using vertex_descriptor =
         boost::graph_traits<GraphType>::vertex_descriptor;
    using vertex_iterator =
         boost::graph_traits<GraphType>::vertex_iterator;
    using edge_iterator =
         boost::graph_traits<GraphType>::edge_iterator;

    std::set<SG::PointType> unique_points;
    std::set<SG::PointType> repeated_points;
    size_t npoints = 0;
    vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(sg);
    for (; vi != vi_end; ++vi) {
        ++npoints;
        auto inserted = unique_points.insert(sg[*vi].pos);
        if(!inserted.second)
            repeated_points.insert(sg[*vi].pos);
    }

    edge_iterator ei, ei_end;
    std::tie(ei, ei_end) = boost::edges(sg);
    for (; ei != ei_end; ++ei) {
        auto &sg_edge = sg[*ei];
        auto &sg_edge_points = sg_edge.edge_points;
        for (size_t index = 0; index < sg_edge_points.size(); ++index) {
            const auto & p = sg_edge_points[index];
            ++npoints;
            auto inserted = unique_points.insert(p);
            if(!inserted.second)
                repeated_points.insert(p);
        }
    }

    bool repeated_exists = (unique_points.size() == npoints) ? false : true;
    return std::make_pair(repeated_points, repeated_exists);
}

} //end namespace
#endif
