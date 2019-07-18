/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef REDUCE_DFS_VISITOR_HPP
#define REDUCE_DFS_VISITOR_HPP
#include "array_utilities.hpp"
#include "split_loop.hpp"
#include <algorithm>
#include <boost/graph/adjacency_iterator.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <iostream>
#include <tuple>

namespace SG {

/**
 *
 * Use DFS (Depth first search/visitor) to remove all nodes with degree 2 and
add them as edge_points of a spatial_edge.
 *
 * This visitor constructs a new spatial_graph with the result.
 *
 * Extra from:
http://www.boost.org/doc/libs/1_65_1/libs/graph/doc/depth_first_search.html

vis.initialize_vertex(s, g) is invoked on every vertex of the graph before the
start of the graph search.

vis.start_vertex(s, g) is invoked on the source vertex once before the start of
the search.

vis.discover_vertex(u, g) is invoked when a vertex is encountered for the first
time.

vis.examine_edge(e, g) is invoked on every out-edge of each vertex after it is
discovered.

vis.tree_edge(e, g) is invoked on each edge as it becomes a member of the edges
that form the search tree. If you wish to record predecessors, do so at this
event point.

vis.back_edge(e, g) is invoked on the back edges in the graph.

vis.forward_or_cross_edge(e, g) is invoked on forward or cross edges in the
graph. In an undirected graph this method is never called.

vis.finish_edge(e, g) is invoked on the non-tree edges in the graph as well as
on each tree edge after its target vertex is finished.

vis.finish_vertex(u, g) is invoked on a vertex after all of its out edges have
been added to the search tree and all of the adjacent vertices have been
discovered (but before their out-edges have been examined).

//  put(color, u, Color::white())
 */
template <typename SpatialGraph, typename VertexMap, typename ColorMap>
struct ReduceGraphVisitor : public boost::default_dfs_visitor {
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

    ReduceGraphVisitor(SpatialGraph &sg,
                       ColorMap &color_map,
                       VertexMap &vertex_map,
                       bool &is_not_loop,
                       bool &verbose)
            : m_sg(sg), m_color_map(color_map), m_vertex_map(vertex_map),
              m_is_not_loop(is_not_loop), m_verbose(verbose) {}

    /**
     * Copy Constructor.
     * DFS visit and search takes a copy of the visitor.
     * Set the boolean reference to false at copy (needed to stop function).
     */
    ReduceGraphVisitor(const ReduceGraphVisitor &other)
            : m_sg(other.m_sg), m_color_map(other.m_color_map),
              m_vertex_map(other.m_vertex_map),
              m_is_not_loop(other.m_is_not_loop), m_verbose(other.m_verbose) {
        m_is_not_loop = false;
    }

    SpatialGraph &m_sg;
    ColorMap &m_color_map;
    VertexMap &m_vertex_map;
    bool &m_is_not_loop;
    bool &m_verbose;

  protected:
    SpatialEdge m_sg_edge;
    static const vertex_descriptor max_vertex_id =
            std::numeric_limits<vertex_descriptor>::max();
    vertex_descriptor m_sg_source = max_vertex_id;
    vertex_descriptor m_source = max_vertex_id;
    bool m_already_started = false;

  public:
    /**
     * invoked when a vertex is encountered for the first time.
     *
     * @param u
     * @param input_sg
     */
    void discover_vertex(vertex_descriptor u,
                         const SpatialGraph &input_sg) { // check!
        auto degree = boost::out_degree(u, input_sg);
        if (m_verbose)
            std::cout << "discover_vertex: " << u << " : "
                      << ArrayUtilities::to_string(input_sg[u].pos)
                      << ". Degree:" << degree << std::endl;
        if (degree != 2) // starting or ending point.
        {
            // Check if current node have an spatial node already created.
            vertex_descriptor sg_vertex_descriptor;
            bool sg_vertex_exists = false;
            auto vertex_search = m_vertex_map.find(u);
            if (vertex_search != m_vertex_map.end()) {
                sg_vertex_exists = true;
                sg_vertex_descriptor = vertex_search->second;
            }

            if (!m_already_started) { // Starting node
                m_already_started = true;
                if (!sg_vertex_exists) {
                    sg_vertex_descriptor = boost::add_vertex(input_sg[u], m_sg);
                }
                // Store the start point to add an edge at the end.
                m_sg_source = sg_vertex_descriptor;
                m_source = u;
            } else { // Ending node
                // Add node if it doesn't exist.
                if (!sg_vertex_exists) {
                    sg_vertex_descriptor = boost::add_vertex(input_sg[u], m_sg);
                }
                // Remove the last edge_point from the spatial
                // edge (already added in the vertex)
                if (!m_sg_edge.edge_points.empty())
                    m_sg_edge.edge_points.pop_back();

                // edge gives one edge (can be any if parallel edges exist.
                auto any_edge_exist =
                        boost::edge(m_sg_source, sg_vertex_descriptor, m_sg);
                if (!any_edge_exist.second) {
                    boost::add_edge(m_sg_source, sg_vertex_descriptor,
                                    m_sg_edge, m_sg);
                    // if(m_verbose)
                    //   std::cout << "Added new edge: (" << m_source << "," <<
                    //   u << ") "
                    //   << m_sg[added.first] << std::endl;
                } else {
                    // Iterate over all parallel edges and checks if any of them
                    // is equal to current edge. If not, add it.
                    auto out_edges = boost::out_edges(m_sg_source, m_sg);
                    auto &ei = out_edges.first;
                    auto &ei_end = out_edges.second;
                    bool current_edge_already_exist = false;
                    for (; ei != ei_end && !current_edge_already_exist; ++ei)
                        if (boost::target(*ei, m_sg) == sg_vertex_descriptor) {
                            auto parallel_edge = *ei;
                            auto &parallel_edge_points =
                                    m_sg[parallel_edge].edge_points;
                            if (parallel_edge_points.size() ==
                                m_sg_edge.edge_points.size()) {
                                auto sorted_current_edge_points =
                                        m_sg_edge.edge_points;
                                std::sort(
                                        std::begin(sorted_current_edge_points),
                                        std::end(sorted_current_edge_points));
                                auto sorted_parallel_edge_points =
                                        parallel_edge_points;
                                std::sort(
                                        std::begin(sorted_parallel_edge_points),
                                        std::end(sorted_parallel_edge_points));
                                if (sorted_current_edge_points ==
                                    sorted_parallel_edge_points) {
                                    // Match, edge exist
                                    current_edge_already_exist = true;
                                }
                            }
                        }
                    if (!current_edge_already_exist) {
                        boost::add_edge(m_sg_source, sg_vertex_descriptor,
                                        m_sg_edge, m_sg);
                        // if(m_verbose)
                        //   std::cout << "Added parallel edge  (" << m_source
                        //   << "," << u
                        //   << ") " << m_sg[added.first] << std::endl;
                    }
                } // end add parallel edge
            }

            if (!sg_vertex_exists)
                m_vertex_map.emplace(u, sg_vertex_descriptor);
            // Visiting should end after this function.
        } // degree check
    }

    /**
     * invoked on each edge as it becomes a member of the edges that form the
     * search tree.
     *
     * @param e
     * @param input_sg
     */
    void tree_edge(edge_descriptor e, const SpatialGraph &input_sg) {
        auto target = boost::target(e, input_sg);

        if (m_verbose)
            std::cout << "tree_edge: " << e << " , target: " << target << " : "
                      << ArrayUtilities::to_string(input_sg[target].pos)
                      << std::endl;
        // It can happen that finish_function is called, but there
        // are still branches in the stack, so the visit won't finish
        // inmediately.
        // We can check that if the source of e is the start vertex,(beggining
        // of a branch) clear the edge_points.
        auto source = boost::source(e, input_sg);
        if (source == m_source)
            m_sg_edge.edge_points.clear();

        m_sg_edge.edge_points.push_back(input_sg[target].pos);
    }

    /**
     * Invoked on the back edges in the graph.
     *
     * Used only for loops. The ending vertex of the loop doesn't use tree_edge.
     * This gets called even after hitting finish_on_junction,
     * because the recursive nature of dfs_visit.
     *
     * @param e
     * @param input_sg
     */
    void back_edge(edge_descriptor e, const SpatialGraph &input_sg) {
        auto target = boost::target(e, input_sg);
        if (m_verbose)
            std::cout << "back_edge: " << e << " , target: " << target << " : "
                      << ArrayUtilities::to_string(input_sg[target].pos)
                      << std::endl;
        // Check if it is a loop and store in the reference. Used by
        // terminate function of the dfs_visit: finish_on_junctions.
        if (!m_is_not_loop && target == m_source &&
            m_sg_edge.edge_points.size() > 1) {
            if (m_verbose)
                std::cout << "Loop: " << target << std::endl;
            m_is_not_loop = false;
            auto vertex_search = m_vertex_map.find(target);
            if (vertex_search != m_vertex_map.end())
                split_loop(vertex_search->second, m_sg_edge, m_sg);
            else
                throw("split loop created new nodes");
        }
    }

    /**
     * Invoked on a vertex after all of its out edges have been added to the
     * search tree and all of the adjacent vertices have been discovered (but
     * before their out-edges have been examined).
     *
     * @param u
     * @param input_sg
     */
    void finish_vertex(vertex_descriptor u, const SpatialGraph &input_sg) {
        using Color =
                typename boost::color_traits<typename ColorMap::mapped_type>;
        using adjacency_iterator =
                typename boost::graph_traits<SpatialGraph>::adjacency_iterator;
        if (m_verbose)
            std::cout << "Finish vertex: " << u << ": "
                      << ArrayUtilities::to_string(input_sg[u].pos)
                      << std::endl;
        // Restore white for vertices with more than 2 degrees.
        // only if u has not visited neighbors (white)
        // For tree_edge to work on them.

        if (boost::out_degree(u, input_sg) > 2) {
            // Check color of neighbors
            adjacency_iterator neighbor_it, neighbor_end_it;
            std::tie(neighbor_it, neighbor_end_it) =
                    boost::adjacent_vertices(u, input_sg);
            bool has_white_neighbors = false;
            for (; neighbor_it != neighbor_end_it; ++neighbor_it) {
                // the map has to be initialized to white for all nodes.
                if (m_color_map[*neighbor_it] == Color::white())
                    has_white_neighbors = true;
            }
            // Restore white
            if (has_white_neighbors)
                m_color_map[u] = Color::white();
        }
    }
};

template <typename SpatialGraph, typename VertexMap, typename ColorMap>
struct SelfLoopGraphVisitor : public boost::default_dfs_visitor {
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

    SelfLoopGraphVisitor(SpatialGraph &sg,
                         ColorMap &color_map,
                         const vertex_descriptor &start,
                         bool &end_visit_flag)
            : m_sg(sg), m_color_map(color_map), m_start(start),
              m_end_visit_flag(end_visit_flag) {}

    SpatialGraph &m_sg;
    ColorMap &m_color_map;
    const vertex_descriptor &m_start;
    bool &m_end_visit_flag;

  protected:
    SpatialEdge m_sg_edge;
    static const vertex_descriptor max_vertex_id =
            std::numeric_limits<vertex_descriptor>::max();
    vertex_descriptor m_sg_source = max_vertex_id;
    vertex_descriptor m_source = max_vertex_id;
    bool m_already_started = false;

  public:
    void discover_vertex(vertex_descriptor u, const SpatialGraph &input_sg) {
        if (boost::out_degree(u, input_sg) == 2 && u == m_start &&
            !this->m_already_started) // starting point
        {
            this->m_already_started = true;
            auto sg_vertex_descriptor =
                    boost::add_vertex(input_sg[u], this->m_sg);
            this->m_sg_source = sg_vertex_descriptor;
            this->m_source = m_start;
        }
    }

    void tree_edge(edge_descriptor e, const SpatialGraph &input_sg) {
        auto target = boost::target(e, input_sg);
        this->m_sg_edge.edge_points.push_back(input_sg[target].pos);
    }

    void back_edge(edge_descriptor e, const SpatialGraph &input_sg) {
        auto target = boost::target(e, input_sg);
        if (target == this->m_source &&
            this->m_sg_edge.edge_points.size() > 2) {
            // std::cout << "Loop: " << target  << std::endl;
            split_loop(this->m_sg_source, this->m_sg_edge, this->m_sg);
            // This will trigger lambda function to end the visit
            this->m_end_visit_flag = true;
            return;
        }
    }
};

} // namespace SG
#endif
