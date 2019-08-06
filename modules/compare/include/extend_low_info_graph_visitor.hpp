/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COMPARE_VISITOR_HPP
#define COMPARE_VISITOR_HPP

#include "array_utilities.hpp"
#include "get_vtk_points_from_graph.hpp"
#include "graph_descriptor.hpp"
#include "graph_points_locator.hpp"
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
 * The original idea of this visitor is to create a result (m_result_sg) graph
 * with the same amount of nodes and edges than the low info graph, but where
 * info from high info graphs is gathered to move the end-points, and extend the
 * edges.
 *
 * The resulting m_result_sg will be the first step to build the final result,
 * where high information is integrated, but
 * the graph should be still precise with high
 * confidence.
 *
 * The result will the scaffold where more branches from high info graphs will
 * be added.
 *
 * @tparam SpatialGraph
 * @tparam VertexMap
 * @tparam ColorMap
 */
template <typename SpatialGraph, typename VertexMap, typename ColorMap>
struct ExtendLowInfoGraphVisitor : public boost::default_dfs_visitor {
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
    using ResultToOriginalVertexMap =
            std::unordered_map<vertex_descriptor,
                               std::vector<vertex_descriptor>>;

    ExtendLowInfoGraphVisitor(
            SpatialGraph &sg,
            const std::vector<std::reference_wrapper<const SpatialGraph>>
                    &graphs,
            IdGraphDescriptorMap &point_id_graphs_map,
            vtkOctreePointLocator *octree,
            double &radius,
            ColorMap &color_map,
            VertexMap &vertex_map,
            bool &verbose)
            : m_result_sg(sg), m_graphs(graphs),
              m_point_id_graphs_map(point_id_graphs_map), m_octree(octree),
              m_radius(radius), m_color_map(color_map),
              m_vertex_map(vertex_map), m_verbose(verbose) {}

    /// The resulting graph
    SpatialGraph &m_result_sg;
    /// The array of graphs ordered from low to high info.
    const std::vector<std::reference_wrapper<const SpatialGraph>> &m_graphs;
    IdGraphDescriptorMap &m_point_id_graphs_map;
    vtkOctreePointLocator *m_octree;
    double &m_radius;
    /// color map to handle which nodes have been visited
    ColorMap &m_color_map;
    /// vertex_descriptor map between input (low-info) graph and the resulting
    /// graph
    VertexMap &m_vertex_map;
    /**
     * Threshold to ignore noisy branches. Number of edge_points.
     * Used in nodes which have no correspondence in high info graphs.
     * TODO: it should be exposed in the constructor?
     */
    size_t m_number_edge_points_to_ignore_low_info_noisy_branch = 10;
    ResultToOriginalVertexMap m_result_to_original_vertex_map;
    bool &m_verbose;
    // TODO Debug variable, remove
    bool m_verbose_extra = false;

    /**
     * invoked when a vertex is encountered for the first time.
     *
     * @param u
     * @param input_sg
     */
    void discover_vertex(vertex_descriptor u, const SpatialGraph &input_sg) {
        auto degree = boost::out_degree(u, input_sg);
        if (m_verbose)
            std::cout << "discover_vertex: " << u << " : "
                      << ArrayUtilities::to_string(input_sg[u].pos)
                      << ". Degree:" << degree << std::endl;
    }

    /**
     * invoked on each edge as it becomes a member of the edges that form the
  search tree
     *
     * We might use examine_edge instead to include back_edges.
     * The goal is to populate the resulting graph with the topology of the low
  graph,
     * but with end-nodes extended by the information from high info graphs.
     *
     * New branches from high info graphs will be ignored. Another visitor will
  be
     * needed to add those. A strategy for that visitor will be to add subtrees
  of a high
     * info graph that only touch the "result" graph once.

     * Implementation:
  1) Add nodes
  Search for graph descriptors around a sphere of radius R on each low info
  node. The goal is to map a node in low graph -gL- with a node in gR, we also
  map that correspondence with all the high info graphs. VertexMap:         node
  in gL -> node in gR ResultToGraphsMap: node in gR -> {node in gL, node in gH1,
  node in gH2, ... }
  - The search with radius R results in:
  a) Another graph descriptor in high info graphs exist in that radius
      - It could be a vertex or an edge.
       - Vertex: If it is vertex, easy, we got our gR node there.
       - Edge:
         - Check also the closest vertex, and compare the distance with the edge
  point. If the distance is small enough (PARAMETER), we take the closest vertex
  as the gR node.
         - If there is no vertex close by, take the edge descriptor associated
  to the edge point. Get source and target nodes of the edge (in gH): We are
  interested in the node (source or target) in gH extending the low info graph.
                - How to choose?

  b) No other graph descriptor exist.
      - Proceed to ignore this branch if it is short (it happens) or throw an
  error... 2) Add Edges
  - Precondition: We have the vertex descriptors of gR (and the map from those
  vertex to low and high info graphs --m_result_to_original_vertex_map) The edge
  to add to gR will be the concatenation of edges needed in gH to connecto those
  two nodes. We use a shortest path search for finding that path.
     *
     * @param e
     * @param input_sg
     */
    void tree_edge(edge_descriptor e, const SpatialGraph &input_sg) {
        auto target = boost::target(e, input_sg);
        auto source = boost::source(e, input_sg);

        if (m_verbose)
            std::cout << "tree_edge: " << e << " , target: " << target << " : "
                      << ArrayUtilities::to_string(input_sg[target].pos)
                      << std::endl;

        std::array<vertex_descriptor, 2> nodes = {source, target};
        std::array<vertex_descriptor, 2> result_sg_nodes;
        for (size_t source_or_target_index = 0;
             source_or_target_index < nodes.size(); ++source_or_target_index) {
            const auto &u = nodes[source_or_target_index];
            if (m_verbose)
                std::cout << "PROCESSING low info u: " << u << std::endl;
            // Check if vertex has been added to the m_result_sg (Check
            // VertexMap)
            vertex_descriptor sg_vertex_descriptor;
            bool sg_vertex_exists = false;
            auto vertex_search = m_vertex_map.find(u);
            if (vertex_search != m_vertex_map.end()) {
                sg_vertex_exists = true;
                sg_vertex_descriptor = vertex_search->second;
            }
            if (sg_vertex_exists) {
                if (m_verbose) {
                    std::cout << "ALREADY ADDED u->m_vertex_map[u]: " << u
                              << "->" << m_vertex_map[u] << std::endl;
                }
                result_sg_nodes[source_or_target_index] = m_vertex_map[u];
                continue; // already added
            }
            // Find the vertices in the other graphs associated to this vertex
            // If there is no vertex, associate it to the source or target of
            // the edge.
            auto closeIdList = graph_closest_points_by_radius_locator(
                    input_sg[u].pos, m_octree, m_point_id_graphs_map, m_radius);
            std::vector<IdWithGraphDescriptor>
                    closest_existing_descriptor_by_graph =
                            closest_existing_descriptors_by_graph(
                                    closeIdList, m_point_id_graphs_map);
            // Get the closest vertex and compare with closest descriptor to
            // check if the vertex has just moved a little bit.
            std::vector<IdWithGraphDescriptor> closest_existing_vert_by_graph =
                    closest_existing_vertex_by_graph(closeIdList,
                                                     m_point_id_graphs_map);

            bool vertex_exists_in_high_info_graphs = true;
            bool vertex_exists_close_by_in_high_info_graphs = false;
            // skip low info graph
            for (size_t index = 1;
                 index < closest_existing_descriptor_by_graph.size(); ++index) {
                const auto &close_desc =
                        closest_existing_descriptor_by_graph[index];
                if (close_desc.exist) {
                    // if the closest descriptor is a vertex
                    if (!(close_desc.descriptor.exist &&
                          close_desc.descriptor.is_vertex)) {
                        vertex_exists_in_high_info_graphs = false;
                        // Check if there is a vertex nearby
                        const auto &vertex_desc =
                                closest_existing_vert_by_graph[index];
                        if (vertex_desc.exist) {
                            const auto &vertex_desc_position =
                                    m_graphs[index]
                                            .get()[vertex_desc.descriptor
                                                           .vertex_d]
                                            .pos;
                            const auto &edge_point_position =
                                    m_graphs[index]
                                            .get()[close_desc.descriptor.edge_d]
                                            .edge_points
                                                    [close_desc.descriptor
                                                             .edge_points_index];
                            auto dist_closest_vertex_and_edge_points =
                                    ArrayUtilities::distance(
                                            vertex_desc_position,
                                            edge_point_position);
                            // TODO this should be an external parameter and
                            // depends on the spacing(positions) of the graph
                            // (if any)
                            double distance_threshold =
                                    m_radius; // Use 2 voxels radius?
                            if (dist_closest_vertex_and_edge_points <=
                                distance_threshold) {
                                vertex_exists_close_by_in_high_info_graphs =
                                        true;
                                vertex_exists_in_high_info_graphs = true;
                            } else {
                                vertex_exists_close_by_in_high_info_graphs =
                                        false;
                                vertex_exists_in_high_info_graphs = false;
                            }
                        }
                    }
                } else { // close_desc does not exist
                    // Shared warning message:
                    std::ostringstream os;
                    os << "WARNING: vertex_descriptor " << u
                       << " in low info graph, with pos ";
                    print_pos(os, input_sg[u].pos);
                    os << std::endl;
                    os << "Cannot be linked to any point in the high info "
                          "graph.\n"
                          "Radius used: "
                       << m_radius << ". You might try to increase the radius."
                       << std::endl;

                    if (boost::degree(u, input_sg) == 1 &&
                        input_sg[e].edge_points.size() <
                                m_number_edge_points_to_ignore_low_info_noisy_branch) {
                        // Do not add this node, or this edge.
                        // The other node of this edge might have been added
                        // already.
                        // TODO: You might need to do a cleanup of vertex with
                        // degree 2 at the end.
                        os << "This node has been ignored because:\n"
                              " - The node is an end-point with degree 1.\n"
                              " - the edge connecting to this node is shorter ("
                           << input_sg[e].edge_points.size()
                           << ") than the parameter"
                              " m_number_edge_points_to_ignore_low_info_noisy_"
                              "branch: "
                           << m_number_edge_points_to_ignore_low_info_noisy_branch
                           << std::endl;
                        std::cout << os.str() << std::endl;
                        return;
                    } else {
                        throw std::runtime_error(os.str());
                    }
                }
            }

            // TODO deal with differences between high info graphs in the future
            // For now,  do the analysis only in the first high info graph:
            // m_graphs[1].get()
            size_t low_graph_index = 0;
            size_t high_graph_index = 1;
            assert(closest_existing_descriptor_by_graph.size() == 2);
            auto &close_desc =
                    closest_existing_descriptor_by_graph[high_graph_index];
            auto &close_vertex_desc =
                    closest_existing_vert_by_graph[high_graph_index];
            if (m_verbose) {
                std::cout << "vertex_descriptor of input graph: " << u
                          << std::endl;
                std::cout << "vertex_exists_in_high_info_graphs: "
                          << vertex_exists_in_high_info_graphs << std::endl;
                std::cout << "vertex_exists_close_by_in_high_info_graphs: "
                          << vertex_exists_close_by_in_high_info_graphs
                          << std::endl;
                print_graph_descriptor(
                        close_desc.descriptor,
                        "gdesc high info graph: " +
                                std::to_string(high_graph_index));
                print_graph_descriptor(
                        close_vertex_desc.descriptor,
                        "gdesc (vertex) high info graph: " +
                                std::to_string(high_graph_index));
            }
            if (vertex_exists_in_high_info_graphs) {
                if (m_verbose)
                    std::cout << u << " IS A VERTEX in high graph" << std::endl;
                // Add node from high info graph to the resulting graph
                vertex_descriptor sg_vertex_descriptor;
                if (!vertex_exists_close_by_in_high_info_graphs) {
                    sg_vertex_descriptor = boost::add_vertex(
                            m_graphs[high_graph_index]
                                    .get()[close_desc.descriptor.vertex_d],
                            this->m_result_sg);
                    add_to_map_result_to_original(
                            sg_vertex_descriptor,
                            close_desc.descriptor.vertex_d, high_graph_index);
                } else {
                    sg_vertex_descriptor = boost::add_vertex(
                            m_graphs[high_graph_index].get()
                                    [close_vertex_desc.descriptor.vertex_d],
                            this->m_result_sg);
                    add_to_map_result_to_original(
                            sg_vertex_descriptor,
                            close_vertex_desc.descriptor.vertex_d,
                            high_graph_index);
                }
                m_vertex_map.emplace(u, sg_vertex_descriptor);
                result_sg_nodes[source_or_target_index] = sg_vertex_descriptor;
                add_to_map_result_to_original(sg_vertex_descriptor, u,
                                              low_graph_index);
            } else { // low info vertex is an edge in high info graph
                if (m_verbose)
                    std::cout << u << " IS AN EDGE in high graph" << std::endl;
                // Add the target or source of that edge as the vertex.
                // How to choose between source or target?
                if (!(close_desc.exist && close_desc.descriptor.is_edge)) {
                    std::cout << "WARNING: point in low graph has no "
                                 "correspondece in high graph"
                              << std::endl;
                    return;
                }

                auto chosen_high_graph_node = choose_between_source_and_target(
                        close_desc.descriptor.edge_d,
                        nodes[source_or_target_index], // u
                        source_or_target_index
                                ? nodes[0]
                                : nodes[1], // target if u == source
                        m_graphs[high_graph_index].get());

                auto sg_vertex_descriptor = boost::add_vertex(
                        m_graphs[high_graph_index]
                                .get()[chosen_high_graph_node],
                        this->m_result_sg);
                m_vertex_map.emplace(u, sg_vertex_descriptor);
                result_sg_nodes[source_or_target_index] = sg_vertex_descriptor;
                add_to_map_result_to_original(sg_vertex_descriptor, u,
                                              low_graph_index);
                add_to_map_result_to_original(sg_vertex_descriptor,
                                              chosen_high_graph_node,
                                              high_graph_index);
            }
        }

        size_t low_graph_index = 0;
        size_t high_graph_index = 1;
        if (m_verbose) {
            std::cout << "Map of nodes between graphs:" << std::endl;
            std::cout << "source: nodes[0]: " << nodes[0] << std::endl;
            std::cout << "target: nodes[1]: " << nodes[1] << std::endl;
            std::cout << "source: result_sg_nodes[0]: " << result_sg_nodes[0]
                      << " -> g0: "
                      << m_result_to_original_vertex_map.at(
                                 result_sg_nodes[0])[low_graph_index]
                      << "; g1: "
                      << m_result_to_original_vertex_map.at(
                                 result_sg_nodes[0])[high_graph_index]
                      << std::endl;
            std::cout << "source: result_sg_nodes[1]: " << result_sg_nodes[1]
                      << " -> g0: "
                      << m_result_to_original_vertex_map.at(
                                 result_sg_nodes[1])[low_graph_index]
                      << "; g1: "
                      << m_result_to_original_vertex_map.at(
                                 result_sg_nodes[1])[high_graph_index]
                      << std::endl;
            std::cout << "...About to add edges..." << std::endl;
        }

        // Add edges, use shortest_path get the path from result_sg_nodes[0] to
        // result_sg_nodes[1] We query the high info graph It might happen that
        // the high info graph has branches between the original source and
        // target, but we ignore them with approach.

        auto high_graph_source = m_result_to_original_vertex_map.at(
                result_sg_nodes[0])[high_graph_index];
        auto high_graph_target = m_result_to_original_vertex_map.at(
                result_sg_nodes[1])[high_graph_index];
        SpatialEdge sg_edge = create_spatial_edge_from_shortest_path(
                high_graph_source, high_graph_target, high_graph_index);

        auto any_edge_exist = boost::edge(result_sg_nodes[0],
                                          result_sg_nodes[1], m_result_sg);
        if (!any_edge_exist.second) {
            auto added_edge =
                    boost::add_edge(result_sg_nodes[0], result_sg_nodes[1],
                                    sg_edge, m_result_sg);
        }
    }

  private:
    void add_to_map_result_to_original(
            const typename ResultToOriginalVertexMap::key_type &key,
            const typename ResultToOriginalVertexMap::mapped_type::value_type
                    &value,
            size_t graph_index) {

        const size_t size_vector = m_graphs.size();
        assert(graph_index < size_vector);
        auto vertex_search = m_result_to_original_vertex_map.find(key);
        // Initialize the vector
        bool vertex_exists = false;
        if (vertex_search != m_result_to_original_vertex_map.end()) {
            vertex_exists = true;
        }
        if (!vertex_exists) {
            std::vector<vertex_descriptor> descriptors(size_vector,
                                                       vertex_descriptor(-1));
            descriptors[graph_index] = value;
            m_result_to_original_vertex_map.emplace(key, descriptors);
        } else {
            // Alias, modify descriptors modifies the map
            auto &descriptors = vertex_search->second;
            if (descriptors[graph_index] != vertex_descriptor(-1)) {
                if (m_verbose) {
                    std::cout << "ALREADY ADDED "
                                 "u->m_result_to_original_vertex_map.at(u)["
                                 "graph_index]: "
                              << key << "->"
                              << m_result_to_original_vertex_map.at(
                                         key)[graph_index]
                              << std::endl;
                }
            } else {
                descriptors[graph_index] = value;
            }
        }
    }

    SpatialEdge
    create_spatial_edge_from_shortest_path(vertex_descriptor high_graph_source,
                                           vertex_descriptor high_graph_target,
                                           size_t high_graph_index) {
        if (m_verbose)
            std::cout << "COMPUTING SHORTEST_PATH...." << std::endl;
        // compute shortest path in high info graph
        auto shortest_path = SG::compute_shortest_path(
                high_graph_source, high_graph_target,
                m_graphs[high_graph_index].get(), m_verbose);
        if (m_verbose) {
            if (shortest_path.empty())
                std::cout << "shortest_path is EMPTY!?" << std::endl;
            for (const auto &v : shortest_path) {
                std::cout << "shortest_path. v: " << v << " ---> ";
                print_pos(std::cout, m_graphs[high_graph_index].get()[v].pos);
                std::cout << std::endl;
            }
            std::vector<edge_descriptor> shortest_path_edge_descriptors;
            for (size_t i = 0; i < shortest_path.size() - 1; ++i) {
                auto edge_pair =
                        boost::edge(shortest_path[i], shortest_path[i + 1],
                                    m_graphs[high_graph_index].get());
                if (!edge_pair.second)
                    std::cout << "WHAAAT? edge doesn't exist? " << std::endl;
                shortest_path_edge_descriptors.push_back(edge_pair.first);
            }
            if (m_verbose_extra) {
                for (const auto &ed : shortest_path_edge_descriptors) {
                    std::cout << "edge: " << ed << " --> ";
                    std::cout << m_graphs[high_graph_index].get()[ed]
                              << std::endl;
                }
            }
        }
        if (m_verbose)
            std::cout << "CREATING EDGE FROM SHORTEST_PATH...." << std::endl;
        SpatialEdge sg_edge = SG::create_edge_from_path(
                shortest_path, m_graphs[high_graph_index].get());
        if (m_verbose)
            std::cout << "EDGE FROM SHORTEST_PATH CREATED:" << std::endl;
        if (m_verbose_extra)
            std::cout << sg_edge << std::endl;

        return sg_edge;
    }

    vertex_descriptor choose_between_source_and_target(
            edge_descriptor high_info_ed,
            vertex_descriptor low_node_to_extend,
            vertex_descriptor low_node_connected_to_node_to_extend,
            const SpatialGraph &high_info_graph) {
        const auto &low_graph = m_graphs[0].get();
        auto high_source = boost::source(high_info_ed, high_info_graph);
        auto high_target = boost::target(high_info_ed, high_info_graph);
        if (m_verbose) {
            std::cout << "Choosing between source and target..." << std::endl;
            std::cout << "high_source: " << high_source << std::endl;
            std::cout << "high_target: " << high_target << std::endl;
        }

        // Check if source and target exist in low G. If only one of them exist,
        // choose the non-existant.
        {} // Check for distances if both, source and target do not exist in low
           // info.
        {
            auto u_pos = low_graph[low_node_to_extend].pos;
            auto high_source_pos = m_graphs[1].get()[high_source].pos;
            auto high_target_pos = m_graphs[1].get()[high_target].pos;
            auto dist_high_source =
                    ArrayUtilities::distance(high_source_pos, u_pos);
            auto dist_high_target =
                    ArrayUtilities::distance(high_target_pos, u_pos);
        }
        // If both exist, choose the closest.
        {}

        // Choose target for now! TODO: BUG, choose wisely instead.
        // return high_target;
        return high_source;
    }
};
} // namespace SG
#endif
