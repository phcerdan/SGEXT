/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SPATIAL_GRAPH_UTILITIES_HPP
#define SPATIAL_GRAPH_UTILITIES_HPP

#include "graph_descriptor.hpp"
#include "spatial_graph.hpp"
#include "spatial_node.hpp"

namespace SG {

/**
 * Return type for get_adjacent_vertices_positions
 * holding vertex_descriptor and positions of adjacent vertices.
 */
struct AdjacentVerticesPositions {
    std::vector<GraphType::vertex_descriptor> neighbours_descriptors;
    std::vector<PointType> neighbours_positions;
};

/**
 * Returns a pair, first a vector with all the points of the graph, and second
 * a vector with the graph_descriptor, associating the points with nodes, edges,
 * and/or the index of the edge_points
 *
 * Similar, but without the vtk dependency to
 * @sa get_vtk_points_from_graph
 *
 * @param graph
 *
 * @return
 */
std::pair<std::vector<SpatialNode::PointType>, std::vector<graph_descriptor> >
get_all_points(const GraphType &graph);

/**
 * Get vertex_descriptors and positions of all adjacent vertices of
 * target node.
 *
 * @param target_node input node to compute adjacent vertices
 * @param g input spatial graph
 *
 * @return struct holding descriptors and positions
 */
AdjacentVerticesPositions
get_adjacent_vertices_positions(const GraphType::vertex_descriptor target_node,
                                const GraphType &g);

void print_pos(std::ostream &out, const SG::SpatialNode::PointType &pos);

template <typename GraphType> size_t num_edge_points(const GraphType &sg) {
    auto edges = boost::edges(sg);
    size_t num_points = 0;
    for (auto ei = edges.first; ei != edges.second; ++ei) {
        for (auto &ep : sg[*ei].edge_points) {
            ++num_points;
        }
    }
    return num_points;
}

template <typename GraphType>
void print_degrees(const GraphType &graph, std::ostream &os = std::cout) {
    os << "Print degrees spatial_graph:" << std::endl;
    os << "Num Vertices: " << boost::num_vertices(graph) << std::endl;
    using vertex_iterator =
            typename boost::graph_traits<GraphType>::vertex_iterator;
    vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(graph);
    for (; vi != vi_end; ++vi) {
        os << *vi << ": " << ArrayUtilities::to_string(graph[*vi].pos)
           << ". Degree: " << boost::out_degree(*vi, graph) << std::endl;
    }
}

template <typename GraphType>
void print_edges(const GraphType &graph, std::ostream &os = std::cout) {
    os << "Print edges spatial_graph:" << std::endl;
    os << "Num Edges: " << boost::num_edges(graph) << std::endl;
    using edge_iterator =
            typename boost::graph_traits<GraphType>::edge_iterator;
    edge_iterator ei, ei_end;
    std::tie(ei, ei_end) = boost::edges(graph);
    for (; ei != ei_end; ++ei) {
        auto source = boost::source(*ei, graph);
        auto target = boost::target(*ei, graph);
        os << source << "---" << target << " ; ";
        print_pos(os, graph[source].pos);
        os << "---";
        print_pos(os, graph[target].pos);
        os << std::endl;
    }
}

template <typename GraphType>
void print_spatial_edges(const GraphType &graph, std::ostream &os = std::cout) {
    os << "Print edges spatial_graph:" << std::endl;
    os << "Num Edges: " << boost::num_edges(graph) << std::endl;
    using edge_iterator =
            typename boost::graph_traits<GraphType>::edge_iterator;
    edge_iterator ei, ei_end;
    std::tie(ei, ei_end) = boost::edges(graph);
    for (; ei != ei_end; ++ei) {
        auto source = boost::source(*ei, graph);
        auto target = boost::target(*ei, graph);
        os << source << "---" << target << " ; ";
        print_pos(os, graph[source].pos);
        os << "---";
        print_pos(os, graph[target].pos);
        os << std::endl;
        os << "edge_points: " << graph[*ei].edge_points.size() << std::endl;
        os << graph[*ei] << std::endl;
    }
}

/**
 * Check the graph has unique points
 *
 * @param sg input spatial graph
 *
 * @return repeated_points, true|false
 */
template <typename GraphType>
std::pair<std::set<PointType>, bool>
check_unique_points_in_graph(const GraphType &sg) {
    using vertex_descriptor =
            typename boost::graph_traits<GraphType>::vertex_descriptor;
    using vertex_iterator =
            typename boost::graph_traits<GraphType>::vertex_iterator;
    using edge_iterator =
            typename boost::graph_traits<GraphType>::edge_iterator;

    std::set<SG::PointType> unique_points;
    std::set<SG::PointType> repeated_points;
    size_t npoints = 0;
    vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(sg);
    for (; vi != vi_end; ++vi) {
        ++npoints;
        auto inserted = unique_points.insert(sg[*vi].pos);
        if (!inserted.second)
            repeated_points.insert(sg[*vi].pos);
    }

    edge_iterator ei, ei_end;
    std::tie(ei, ei_end) = boost::edges(sg);
    for (; ei != ei_end; ++ei) {
        auto &sg_edge = sg[*ei];
        auto &sg_edge_points = sg_edge.edge_points;
        for (size_t index = 0; index < sg_edge_points.size(); ++index) {
            const auto &p = sg_edge_points[index];
            ++npoints;
            auto inserted = unique_points.insert(p);
            if (!inserted.second)
                repeated_points.insert(p);
        }
    }

    bool repeated_exists = (unique_points.size() == npoints) ? false : true;
    return std::make_pair(repeated_points, repeated_exists);
}
} // namespace SG
#endif
