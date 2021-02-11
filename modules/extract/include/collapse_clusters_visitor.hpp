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

#ifndef SGEXT_COLLAPSE_CLUSTERS_VISITOR_HPP
#define SGEXT_COLLAPSE_CLUSTERS_VISITOR_HPP

#include "array_utilities.hpp"
#include "shortest_path.hpp"
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
 * Use DFS (Depth first search/visitor) to merge/cluster all vertices that are
 * close together using the geometrical information of the spatial graph.
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

 */
template <typename SpatialGraph, typename VertexMap, typename EdgeAddedMap>
struct CollapseClustersGraphVisitor : public boost::default_dfs_visitor {
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
    using LabelType = vertex_descriptor;
    using ClusterSingleLabelMap =
            std::unordered_map<vertex_descriptor, LabelType>;
    // using EdgeAddedMap = std::map<edge_descriptor, bool>;

    CollapseClustersGraphVisitor(SpatialGraph &sg_out,
                                 VertexMap &vertex_map,
                                 EdgeAddedMap &edge_added_map,
                                 const ClusterSingleLabelMap &cluster_label_map,
                                 bool &verbose)
            : m_sg_out(sg_out), m_vertex_map(vertex_map),
              m_edge_added_map(edge_added_map),
              m_cluster_label_map(cluster_label_map), m_verbose(verbose) {}
    /**
     * Copy Constructor.
     * DFS visit and search takes a copy of the visitor.
     * Set the boolean reference to false at copy (needed to stop function).
     */
    CollapseClustersGraphVisitor(const CollapseClustersGraphVisitor &other)
            : boost::default_dfs_visitor(other), m_sg_out(other.m_sg_out),
              m_vertex_map(other.m_vertex_map),
              m_edge_added_map(other.m_edge_added_map),
              m_cluster_label_map(other.m_cluster_label_map),
              m_verbose(other.m_verbose) {}

    /** output graph the is constructed during the visit */
    SpatialGraph &m_sg_out;
    /** vertex map between the input_spatial_graph and the output (m_sg_out) */
    VertexMap &m_vertex_map;
    /** Map to check if edge is already added. It allows to add parallel edges.
     */
    EdgeAddedMap &m_edge_added_map;
    /** vertex to cluster label map. Obtained from detect_clusters. */
    const ClusterSingleLabelMap &m_cluster_label_map;
    bool &m_verbose;

    struct belongs_to_cluster_result {
        bool belongs_to_a_cluster;
        vertex_descriptor input_vertex_descriptor;
        LabelType cluster_label;
    };

    belongs_to_cluster_result belongs_to_cluster(vertex_descriptor u) const {
        // Check if current node is in a cluster
        // Note that the cluster label can be the smallest vertex_descriptor of
        // the cluster, or the vertex_descriptor corresponding to the centroid
        // of the cluster or any other criteria (being an existing
        // vertex_descriptor).
        LabelType cluster_label = std::numeric_limits<LabelType>::max();
        bool cluster_vertex = false;
        auto cluster_search = m_cluster_label_map.find(u);
        if (cluster_search != m_cluster_label_map.end()) {
            cluster_vertex = true;
            cluster_label = cluster_search->second;
        }

        // Get the right input_vertex_descriptor
        vertex_descriptor input_vertex_descriptor;
        if (!cluster_vertex) {
            input_vertex_descriptor = u;
        } else {
            input_vertex_descriptor = cluster_label;
        }
        return belongs_to_cluster_result(
                {cluster_vertex, input_vertex_descriptor, cluster_label});
    }

    std::pair<bool, vertex_descriptor>
    add_node_to_output_if_not_exist(vertex_descriptor u,
                                    const SpatialGraph &input_sg) {
        // Check if input_vertex_descriptor node was added to the output
        // already,
        vertex_descriptor out_vertex_descriptor =
                std::numeric_limits<vertex_descriptor>::max();
        bool out_vertex_exists = false;
        auto out_vertex_search = m_vertex_map.find(u);
        if (out_vertex_search != m_vertex_map.end()) {
            out_vertex_exists = true;
            out_vertex_descriptor = out_vertex_search->second;
        }
        // If the node (or the cluster node) wasn't added, do it.
        if (!out_vertex_exists) {
            out_vertex_descriptor = boost::add_vertex(input_sg[u], m_sg_out);
            m_vertex_map.emplace(u, out_vertex_descriptor);
        }
        const bool vertex_added = !out_vertex_exists;
        return std::make_pair(vertex_added, out_vertex_descriptor);
    }

    bool edge_exists_in_output(edge_descriptor e) {
        if (m_edge_added_map.find(e) == m_edge_added_map.end()) {
            return false;
        }
        return true;
    }

    /**
     * invoked when a vertex is encountered for the first time.
     *
     * @param u
     * @param input_sg
     */
    void discover_vertex(vertex_descriptor u, const SpatialGraph &input_sg) {
        const auto belongs_to_cluster_result = belongs_to_cluster(u);
        const auto &input_vertex_descriptor =
                belongs_to_cluster_result.input_vertex_descriptor;
        if (m_verbose) {
            std::cout << "collapse discover_vertex: " << u << " : "
                      << ArrayUtilities::to_string(input_sg[u].pos)
                      << std::endl;
        }
        add_node_to_output_if_not_exist(input_vertex_descriptor, input_sg);
        // TODO: Move the spatial_node in the output graph to the center of mass
        // of the cluster or
        // choose a node that represents the cluster with some
        // criteria. for example: being the closer to the center of mass.
        // The latest is available in detect_clusters_with_radius
    }

    /**
     * invoked on each edge as it becomes a member of the edges that form
     * the search tree.
     *
     * If the edge is internal between the cluster, ignore it
     * If the edge link a cluster with something else (a non-cluster node,
     * or a node from another cluster) add the edge in the output with the right
     * descriptors.
     *
     * @param e
     * @param input_sg
     */
    void examine_edge(edge_descriptor e, const SpatialGraph &input_sg) {
        const auto source = boost::source(e, input_sg);
        const auto target = boost::target(e, input_sg);
        const auto source_belongs_to_cluster_result =
                belongs_to_cluster(source);
        const auto target_belongs_to_cluster_result =
                belongs_to_cluster(target);
        if (m_verbose) {
            std::cout << "collapse examine_edge: " << e
                      << " , target: " << target << " : "
                      << ArrayUtilities::to_string(input_sg[target].pos)
                      << std::endl;
        }
        // Internal cluster edge, do nothing.
        if (source_belongs_to_cluster_result.belongs_to_a_cluster &&
            source_belongs_to_cluster_result.cluster_label ==
                    target_belongs_to_cluster_result.cluster_label) {
            if (m_verbose) {
                std::cout << " -- internal cluster edge, edge ignored. "
                          << std::endl;
            }
            return;
        }
        vertex_descriptor output_source_descriptor;
        std::tie(std::ignore, output_source_descriptor) =
                add_node_to_output_if_not_exist(
                        source_belongs_to_cluster_result
                                .input_vertex_descriptor,
                        input_sg);

        vertex_descriptor output_target_descriptor;
        std::tie(std::ignore, output_target_descriptor) =
                add_node_to_output_if_not_exist(
                        target_belongs_to_cluster_result
                                .input_vertex_descriptor,
                        input_sg);

        // add edges
        // TODO Implement voxelization of a line:
        // http://vtk.1045678.n5.nabble.com/Solid-Voxelization-with-VTK-td5747520.html
        // and add the points (voxels) to the spatial edge.
        // Or, follow the shortest path, and copy all the points involved.
        // The latest is implemented.
        if (edge_exists_in_output(e)) {
            return;
        }

        auto spatial_edge = SpatialEdge();
        const bool edge_connecting_non_clusters =
                !source_belongs_to_cluster_result.belongs_to_a_cluster &&
                !target_belongs_to_cluster_result.belongs_to_a_cluster;
        if (edge_connecting_non_clusters) {
            // Keep the existing spatial edge
            spatial_edge = input_sg[e];
        } else {
            // Compute the shortest path and build the the edge from it.
            // For more correct results, be sure than the labels representing
            // the vertex clusters are close to the centroid of the cluster.
            const auto vertex_path = compute_shortest_path(
                    source_belongs_to_cluster_result.input_vertex_descriptor,
                    target_belongs_to_cluster_result.input_vertex_descriptor,
                    input_sg, m_verbose);
            spatial_edge = create_edge_from_path(vertex_path, input_sg);
        }
        boost::add_edge(output_source_descriptor, output_target_descriptor,
                        spatial_edge, m_sg_out);
        m_edge_added_map.emplace(e, true);
        if (m_verbose) {
            std::cout << " -- added spatial_edge with edge_points: ";
            print_edge_points(spatial_edge.edge_points, std::cout);
            std::cout << std::endl;
        }
    }
};
} // end namespace SG
#endif
