/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SPATIAL_GRAPH_DIFFERENCE_VISITOR_HPP
#define SPATIAL_GRAPH_DIFFERENCE_VISITOR_HPP

#include "array_utilities.hpp"
#include "get_vtk_points_from_graph.hpp"
#include "graph_descriptor.hpp"
#include "graph_points_locator.hpp"
#include "print_locator_points.hpp"
#include "shortest_path.hpp"
#include "spatial_graph_utilities.hpp"
#include <algorithm>
#include <boost/graph/adjacency_iterator.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <iostream>
#include <tuple>
#include <vtkIdList.h>
#include <vtkOctreePointLocator.h>

namespace SG {

/**
 *
 * @tparam SpatialGraph
 * @tparam VertexMap
 * @tparam ColorMap
 */
template <typename SpatialGraph, typename VertexMap, typename ColorMap>
struct SpatialGraphDifferenceVisitor : public boost::default_dfs_visitor {
    using SpatialGraphVertexBundle =
            typename boost::vertex_bundle_type<SpatialGraph>::type;
    using SpatialVertex = SpatialGraphVertexBundle;
    using SpatialGraphEdgeBundle =
            typename boost::edge_bundle_type<SpatialGraph>::type;
    using SpatialEdge = SpatialGraphEdgeBundle;
    using vertex_descriptor =
            typename boost::graph_traits<SpatialGraph>::vertex_descriptor;
    using edge_descriptor =
            typename boost::graph_traits<SpatialGraph>::edge_descriptor;

    SpatialGraphDifferenceVisitor(
            SpatialGraph &result_sg,           // result D
            const SpatialGraph &substraend_sg, // S in D = M - S
            const IdGraphDescriptorMap &point_id_graphs_map,
            vtkOctreePointLocator *octree,
            double &radius,
            ColorMap &color_map,
            VertexMap &vertex_map,
            bool &verbose)
            : m_result_sg(result_sg), m_substraend_sg(substraend_sg),
              m_point_id_graphs_map(point_id_graphs_map), m_octree(octree),
              m_radius(radius), m_color_map(color_map),
              m_vertex_map(vertex_map), m_verbose(verbose) {}

    /// The resulting graph D
    SpatialGraph &m_result_sg;
    /// Graph S in: D = M - S
    const SpatialGraph &m_substraend_sg;
    /// point id to graph descriptors
    const IdGraphDescriptorMap &m_point_id_graphs_map;
    /// point locator, initialized to contain points from M y S
    vtkOctreePointLocator *m_octree;
    /// radius of the sphere used in the octre search for close points
    double &m_radius;
    /// color map to handle which nodes have been visited
    ColorMap &m_color_map;
    /// Map between vertex of input and the resulting graph (m_result_sg)
    VertexMap &m_vertex_map;
    bool &m_verbose;

    /**
     * invoked when a vertex is encountered for the first time.
     *
     * @param u
     * @param input_sg
     */
    void discover_vertex(vertex_descriptor u, const SpatialGraph &input_sg) {
        auto degree = boost::out_degree(u, input_sg);
        if (m_verbose) {
            std::cout << "SpatialGraphDifference. discover_vertex: " << u
                      << " : " << ArrayUtilities::to_string(input_sg[u].pos)
                      << ". Degree:" << degree << std::endl;
        }
        // Check if current node exist in the result graph already.
        vertex_descriptor sg_vertex_descriptor;
        bool sg_vertex_exists = false;
        auto vertex_search = m_vertex_map.find(u);
        if (vertex_search != m_vertex_map.end()) {
            sg_vertex_exists = true;
            sg_vertex_descriptor = vertex_search->second;
        }
        // Add new vertex if it doesn't exist in substraend graph
        // If it exists, it might still be added if an edge needs it when
        // processing edges
        if (!sg_vertex_exists) {
            add_new_vertex_if_does_not_exist_in_substraend_graph(u, input_sg);
        }
    }

    /**
     * Precondition: Vertex that does exist in input_sg (minuend), but not in
     * substraend has been added in discover_vertex (and also added to
     * m_vertex_map)
     *
     * Goal:
     * Check both nodes of the edge.
     * A) Both nodes do NOT exist in substraend_sg.
     *  - Add edge (both nodes have been added in discover_vertex already)
     * B) Both nodes do exist in substraend_sg
     *  Check if there is an edge in substraend graph between the same
     * positions.
     *  - if(substraend_edge_exists): Do nothing
     *  - else(): Add both nodes plus the edge.
     * C) Only one of the nodes exist in substraend_sg.
     *  - add node existing in substraend_sg, plus add edge
     *
     * @param e
     * @param input_sg
     */
    void examine_edge(edge_descriptor e, const SpatialGraph &input_sg) {
        auto source = boost::source(e, input_sg);
        auto target = boost::target(e, input_sg);
        if (m_verbose) {
            std::cout << "SpatialGraphDifference. examine_edge: " << e
                      << " , target: " << target << " : "
                      << ArrayUtilities::to_string(input_sg[target].pos)
                      << std::endl;
        }

        // sort (assumes undirected graph)
        if (target < source) {
            std::swap(target, source);
        }
        std::vector<vertex_descriptor> input_nodes_sorted = {source, target};
        std::vector<vertex_descriptor> result_nodes(2);

        // Add both nodes if the edge has to be added. The edge needs to be
        // added if:
        // - Any of source/target DOES NOT exist in substraend graph. DONE
        // - Both source/target exist in substraend, but the edge between them
        // does not.
        std::array<bool, 2> input_nodes_exist_in_result_graph = {false, false};
        std::array<bool, 2> input_nodes_exist_in_substraend_graph = {false,
                                                                     false};
        for (unsigned int source_target_index = 0;
             source_target_index < input_nodes_sorted.size();
             ++source_target_index) {
            const vertex_descriptor &u =
                    input_nodes_sorted[source_target_index];
            // Check if current node exist in the result graph already.
            auto vertex_search = m_vertex_map.find(u);
            if (vertex_search != m_vertex_map.end()) {
                input_nodes_exist_in_result_graph[source_target_index] = true;
                result_nodes[source_target_index] = vertex_search->second;
            }
        }

        const size_t count_input_nodes_existing_in_result_graph =
                std::count(input_nodes_exist_in_result_graph.cbegin(),
                           input_nodes_exist_in_result_graph.cend(), true);

        const bool source_or_target_are_in_result_graph =
                (count_input_nodes_existing_in_result_graph > 0);

        const bool source_or_target_are_not_in_result_graph =
                (count_input_nodes_existing_in_result_graph < 2);

        const bool source_and_target_are_in_result_graph =
                (count_input_nodes_existing_in_result_graph == 2);

        const bool only_one_source_or_target_is_in_result_graph =
                (count_input_nodes_existing_in_result_graph == 1);

        const bool source_and_target_are_not_in_result_graph =
                (count_input_nodes_existing_in_result_graph == 0);

        const size_t minuend_index = 0;
        const size_t substraend_index = 1;
        auto closest_desc_source = get_closest_existing_descriptors(
                input_sg[input_nodes_sorted[0]].pos);
        auto closest_desc_target = get_closest_existing_descriptors(
                input_sg[input_nodes_sorted[1]].pos);
        // If nodes exists in both graphs, but there is no edge in substraend:
        // - Add both nodes, and the existing edge from input_sg
        if (source_and_target_are_not_in_result_graph) {
            if (closest_desc_source[substraend_index].descriptor.is_vertex &&
                closest_desc_target[substraend_index].descriptor.is_vertex) {
                auto any_edge_exist_minuend = boost::edge(
                        input_nodes_sorted[0], input_nodes_sorted[1], input_sg);
                if (any_edge_exist_minuend.second) {
                    const auto &substraend_source =
                            closest_desc_source[substraend_index];
                    const auto &substraend_target =
                            closest_desc_target[substraend_index];
                    auto any_edge_exist_substraend =
                            boost::edge(substraend_source.descriptor.vertex_d,
                                        substraend_target.descriptor.vertex_d,
                                        m_substraend_sg);
                    if (!any_edge_exist_substraend.second) {
                        // Edge exist in minuend , but no in substraend.
                        // Add both nodes and edge
                        result_nodes[0] = boost::add_vertex(
                                input_sg[input_nodes_sorted[0]], m_result_sg);
                        result_nodes[1] = boost::add_vertex(
                                input_sg[input_nodes_sorted[1]], m_result_sg);
                        m_vertex_map.emplace(input_nodes_sorted[0],
                                             result_nodes[0]);
                        m_vertex_map.emplace(input_nodes_sorted[1],
                                             result_nodes[1]);
                        boost::add_edge(result_nodes[0], result_nodes[1],
                                        input_sg[e], m_result_sg);
                        if (m_verbose) {
                            std::cout << "ADD BOTH VERTEX: "
                                      << input_nodes_sorted[0] << " and "
                                      << input_nodes_sorted[1] << std::endl;
                            std::cout << "AND ADD_EDGE ";
                            std::cout << result_nodes[0] << ", "
                                      << result_nodes[1] << std::endl;
                        }
                        // All done for this edge
                        return;
                    }
                }
            }
        }
        // If neither of the nodes are in the result graph, no edge to add.
        if (source_and_target_are_not_in_result_graph)
            return;

        // From here, source_or_target_are_in_result_graph is assumed to be
        // true. At least one of the nodes is in the result graph

        // If one of nodes of the edge exist, but not the other.
        // And edge does not exist in substraend graph,
        // then add the edge
        if (only_one_source_or_target_is_in_result_graph) {
            bool substraend_edge_exists = false;
            if (closest_desc_source[substraend_index].descriptor.is_vertex &&
                closest_desc_target[substraend_index].descriptor.is_vertex) {
                const auto &substraend_source =
                        closest_desc_source[substraend_index];
                const auto &substraend_target =
                        closest_desc_target[substraend_index];
                auto any_edge_exist_substraend = boost::edge(
                        substraend_source.descriptor.vertex_d,
                        substraend_target.descriptor.vertex_d, m_substraend_sg);
                if (any_edge_exist_substraend.second) {
                    substraend_edge_exists = true;
                }
            }
            if (!substraend_edge_exists) {
                // Find the non-existant vertex:
                auto find_result = std::find(
                        std::begin(input_nodes_exist_in_result_graph),
                        std::end(input_nodes_exist_in_result_graph), false);
                assert(find_result !=
                       std::end(input_nodes_exist_in_result_graph));
                auto index_of_non_existant_vertex = std::distance(
                        std::begin(input_nodes_exist_in_result_graph),
                        find_result);
                auto non_existant_vertex =
                        input_nodes_sorted[index_of_non_existant_vertex];

                result_nodes[index_of_non_existant_vertex] = boost::add_vertex(
                        input_sg[non_existant_vertex], m_result_sg);
                m_vertex_map.emplace(
                        non_existant_vertex,
                        result_nodes[index_of_non_existant_vertex]);
                if (m_verbose) {
                    std::cout << "VERTEX ADDED in examine_edge: "
                              << non_existant_vertex << std::endl;
                }
            }
        }
        // result_nodes is now populated

        // Add edge
        {
            // Check edge has not been added before
            auto any_edge_exist_result =
                    boost::edge(result_nodes[0], result_nodes[1], m_result_sg);
            // Add edge between result graph nodes
            // CASE A: The whole edge from input_sg (high info)
            if (!any_edge_exist_result.second) {
                // Check if exist in substraend to avoid adding it
                auto &substraend_source = closest_desc_source[substraend_index];
                auto &substraend_target = closest_desc_target[substraend_index];
                if (substraend_source.exist &&
                    substraend_source.descriptor.is_vertex) {
                    if (substraend_target.exist &&
                        substraend_target.descriptor.is_vertex) {
                        // Both are also vertex in substraend graph,
                        // add edge only if there is no edge in substraend
                        // graph.
                        auto any_edge_exist_substraend = boost::edge(
                                substraend_source.descriptor.vertex_d,
                                substraend_target.descriptor.vertex_d,
                                m_substraend_sg);
                        if (!any_edge_exist_substraend.second) {
                            if (m_verbose) {
                                std::cout << "ADD_EDGE (both vertex exists in "
                                             "substraend, but "
                                             "edge doesn't): ";
                                std::cout << result_nodes[0] << ", "
                                          << result_nodes[1] << std::endl;
                            }
                            boost::add_edge(result_nodes[0], result_nodes[1],
                                            input_sg[e], m_result_sg);
                        }
                    }
                }

                if (substraend_source.exist &&
                    substraend_source.descriptor.is_edge) {
                    if (m_verbose)
                        std::cout << "Substraend Source: is_edge" << std::endl;
                }

                if (!substraend_source.exist || !substraend_target.exist) {
                    if (m_verbose) {
                        std::cout << "ADD_EDGE (any of the vertex does not "
                                     "exist in "
                                     "substraend): ";
                        std::cout << result_nodes[0] << ", " << result_nodes[1]
                                  << std::endl;
                    }
                    boost::add_edge(result_nodes[0], result_nodes[1],
                                    input_sg[e], m_result_sg);
                }
            }
        }
    }

  private:
    std::vector<IdWithGraphDescriptor>
    get_closest_existing_descriptors(const SG::PointType &pos) {
        auto closeIdList = graph_closest_points_by_radius_locator(
                pos, m_octree, m_point_id_graphs_map, m_radius);
        return closest_existing_descriptors_by_graph(closeIdList,
                                                     m_point_id_graphs_map);
    }

    std::pair<bool, graph_descriptor>
    point_exist_in_substraend_graph(const SG::PointType &pos) {
        std::vector<IdWithGraphDescriptor>
                closest_existing_descriptor_by_graph =
                        get_closest_existing_descriptors(pos);
        // The idMap should be constructed with the first index corresponding to
        // the input graph (minuend_sg) and the last index corresponding to
        // substraend_sg
        assert(closest_existing_descriptor_by_graph.size() == 2);
        // SG::print_graph_descriptor(
        //     closest_existing_descriptor_by_graph[0].descriptor,
        //     "gdesc_minuend");
        // SG::print_graph_descriptor(
        //     closest_existing_descriptor_by_graph[1].descriptor,
        //     "gdesc_substraend");
        if (!closest_existing_descriptor_by_graph[1].exist) {
            return std::make_pair(false, graph_descriptor());
        } else {
            return std::make_pair(
                    true, closest_existing_descriptor_by_graph[1].descriptor);
        }
    }

    std::pair<bool, vertex_descriptor>
    add_new_vertex_if_does_not_exist_in_substraend_graph(
            vertex_descriptor u, const SpatialGraph &input_sg) {
        vertex_descriptor sg_vertex_descriptor;
        bool added = false;
        auto &input_spatial_node = input_sg[u];
        bool exist_in_substraend_graph;
        graph_descriptor gdesc_substraend_graph;
        std::tie(exist_in_substraend_graph, gdesc_substraend_graph) =
                point_exist_in_substraend_graph(input_spatial_node.pos);
        // Does not exist in substraend graph
        // If it exists, it might still be added if an edge needs it.
        if (m_verbose && exist_in_substraend_graph) {
            std::cout << "point_exist_in_substraend_graph: graph descriptor "
                         "does exist"
                      << std::endl;
        } else {
            std::cout << "point_exist_in_substraend_graph: graph descriptor "
                         "does NOT exist"
                      << std::endl;
        }
        if (!exist_in_substraend_graph) {
            sg_vertex_descriptor =
                    boost::add_vertex(input_spatial_node, m_result_sg);
            added = true;
            if (m_verbose)
                std::cout << "VERTEX ADDED " << u << std::endl;
            m_vertex_map.emplace(u, sg_vertex_descriptor);
        }
        return std::make_pair(added, sg_vertex_descriptor);
    }
};
} // namespace SG
#endif
