/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SGEXT_DETECT_CLUSTERS_VISITOR_HPP
#define SGEXT_DETECT_CLUSTERS_VISITOR_HPP

#include "array_utilities.hpp"
#include "edge_points_utilities.hpp" // For SG::ete_distance
#include <algorithm>
#include <boost/graph/adjacency_iterator.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <iostream>
#include <tuple>

namespace SG {

inline SG::PointType
get_centroid(const std::set<SG::GraphType::vertex_descriptor> &cluster_vertices,
             const GraphType &input_sg) {
    SG::PointType center;
    for (const auto cluster_vertex : cluster_vertices) {
        center = ArrayUtilities::plus(center, input_sg[cluster_vertex].pos);
    }
    return ArrayUtilities::product_scalar(
            center, 1.0 / static_cast<double>(cluster_vertices.size()));
}

inline SG::GraphType::vertex_descriptor get_vertex_closer_to_centroid(
        const std::set<SG::GraphType::vertex_descriptor> &cluster_vertices,
        const GraphType &input_sg) {

    // First compute centroid of the vertex cloud
    const auto center = get_centroid(cluster_vertices, input_sg);

    // Choose the node closer to the center.
    using vertex_descriptor = SG::GraphType::vertex_descriptor;
    double min_distance = std::numeric_limits<double>::max();
    vertex_descriptor closer_vertex_descriptor =
            std::numeric_limits<vertex_descriptor>::max();
    for (const auto cluster_vertex : cluster_vertices) {
        const auto dist =
                ArrayUtilities::distance(input_sg[cluster_vertex].pos, center);
        if (dist < min_distance) {
            min_distance = dist;
            closer_vertex_descriptor = cluster_vertex;
        }
    }
    return closer_vertex_descriptor;
}

/**
 * Use DFS (Depth first search/visitor) to detect clusters in input graph. A
 * custom function to detect clusters can be provided, but the default uses the
 * geometrical information of the spatial graphand an input radius to consider a
 * cluster based on proximity with its neighbors.
 *
 * In this case with the radius, two nodes can belong to the same cluster, even
 * though they are further away than the input radius.
 *
 * @sa detect_clusters_with_radius for a ready to use function.
 *
 * For collapsing the clusters into a new graph (removing internal cluster
 * nodes/edges) @sa collapse_clusters @sa collapse_clusters_visitor.
 * The collapse function use the map obtained from
 * @ref get_clean_cluster_label_map
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
template <typename SpatialGraph>
struct DetectClustersGraphVisitor : public boost::default_dfs_visitor {
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
    /** Condition to detect clusters using spatial graph edges.  */
    using ClusterEdgeCondition =
            std::function<bool(edge_descriptor, const SpatialGraph &input_sg)>;
    /** Keep a map to label clusters in the graph.
     * A cluster is defined as a subgraph where some nodes can be merged,
     * because they are close between each other, or other conditions.
     * After identifying a mergeable cluster, extra logic is needed to
     * choose what nodes/edges to keep, usually this is an asymmetric decision.
     * */
    using VertexToClusterMap =
            std::unordered_map<vertex_descriptor, std::set<vertex_descriptor>>;

    DetectClustersGraphVisitor(VertexToClusterMap &vertex_to_cluster_map,
                               ClusterEdgeCondition edge_condition,
                               bool &verbose)
            : m_vertex_to_cluster_map(vertex_to_cluster_map),
              m_cluster_edge_condition(edge_condition), m_verbose(verbose) {}
    /**
     * Copy Constructor.
     * DFS visit and search takes a copy of the visitor.
     */
    DetectClustersGraphVisitor(const DetectClustersGraphVisitor &other)
            : m_vertex_to_cluster_map(other.m_vertex_to_cluster_map),
              m_cluster_edge_condition(other.m_cluster_edge_condition),
              m_verbose(other.m_verbose){};

    /** Map vertex_descriptor to the set of vertex_descriptors that belongs to a
     * cluster. */
    VertexToClusterMap &m_vertex_to_cluster_map;
    /**
     * Function that mark the two nodes of an edge as belonging to a cluster.
     * @sa condition_edge_is_close
     * */
    ClusterEdgeCondition m_cluster_edge_condition;
    bool &m_verbose;

    static bool condition_edge_is_close(edge_descriptor e,
                                        const SpatialGraph &input_sg,
                                        const double &radius_proximity) {
        const auto ete_distance = SG::ete_distance(e, input_sg);
        return (ete_distance <= radius_proximity) ? true : false;
    }

    /**
     * VertexToSingleLabelClusterMap is a cleaner version of VertexToClusterMap.
     * It only contains real clusters (those with more than one vertex)
     */
    using LabelType = size_t;
    using VertexToSingleLabelClusterMap =
            std::unordered_map<vertex_descriptor, LabelType>;

  protected:
    /**
     * Add pair (key:vertex, value:cluster_id) to the vertex_to_cluster_map.
     * If the key doesn't exist in the map yet, emplace itself as a cluster_id.
     *
     * @param u input vertex (key)
     * @param cluster_id input cluster_id (value)
     *
     * @return true if new key was emplaced (ie. !vertex_already_exists_as_key)
     */
    bool add_to_vertex_to_cluster_map(vertex_descriptor u_key,
                                      vertex_descriptor cluster_id) {
        // Add vertex to VertexToClusterMap
        const bool vertex_already_exists_as_key =
                (m_vertex_to_cluster_map.find(u_key) !=
                 m_vertex_to_cluster_map.end())
                        ? true
                        : false;
        // Add the key itself as a cluster_id if the key doesn't exist yet
        if (!vertex_already_exists_as_key) {
            std::set<vertex_descriptor> cluster_vertices;
            cluster_vertices.insert(u_key);
            const auto emplace_it_bool =
                    m_vertex_to_cluster_map.emplace(u_key, cluster_vertices);
            if (m_verbose && emplace_it_bool.second) {
                std::cout << "  Adding key " << u_key << " to map" << std::endl;
            }
        }

        if (u_key != cluster_id) {
            // And now add the cluster_id
            std::set<vertex_descriptor> &cluster_vertices =
                    m_vertex_to_cluster_map.at(u_key);
            const auto insert_it_bool_pair =
                    cluster_vertices.insert(cluster_id);
            if (m_verbose && insert_it_bool_pair.second) {
                std::cout << "  Appending value " << cluster_id << " to key "
                          << u_key << std::endl;
            }
        }

        return !vertex_already_exists_as_key;
    };

  public:
    /**
     * invoked when a vertex is encountered for the first time.
     *
     * @param u
     * @param input_sg
     */
    void discover_vertex(vertex_descriptor u, const SpatialGraph &input_sg) {
        if (m_verbose) {
            std::cout << "discover_vertex: " << u << " : "
                      << ArrayUtilities::to_string(input_sg[u].pos)
                      << std::endl;
        }
        add_to_vertex_to_cluster_map(u, u);
    }

    /**
     * invoked on each edge as it becomes a member of the
     * edges that form the search tree. If you wish to record predecessors, do
     * so at this event point.
     *
     * @param e
     * @param input_sg
     */
    void tree_edge(edge_descriptor e, const SpatialGraph &input_sg) {
        const auto target = boost::target(e, input_sg);
        const bool are_both_vertices_in_the_same_cluster =
                m_cluster_edge_condition(e, input_sg);
        if (m_verbose) {
            std::cout << "tree_edge: " << e << " , target: " << target << " : "
                      << ArrayUtilities::to_string(input_sg[target].pos)
                      << " | edge_condition_true? "
                      << (are_both_vertices_in_the_same_cluster ? "yes" : "no")
                      << std::endl;
        }
        if (are_both_vertices_in_the_same_cluster) {
            const auto source = boost::source(e, input_sg);
            add_to_vertex_to_cluster_map(source, target);
            add_to_vertex_to_cluster_map(target, source);
        }
    }

    struct SingleLabelMaps {
        using LabelToVertexMap =
                std::unordered_map<LabelType, vertex_descriptor>;
        VertexToSingleLabelClusterMap vertex_to_single_label_cluster_map;
        LabelToVertexMap label_to_vertex_representing_cluster_map;
    };

    /**
     * To be called after the visit has finished.
     *
     * Copy the vertex_to_cluster_map, but removing all the "non-clusters",
     * those where the size of the set is one. For real clusters, keep only one
     * label in the set, instead of a set.
     *
     * If choose_label_closer_to_cluster_centroid is true,
     * the chosen label to represent the cluster is the closest to the centroid
     * of the cluster. Please note that being the closest doesn't mean is IN the
     * center.
     */
    SingleLabelMaps get_single_label_cluster_maps() {
        SingleLabelMaps result; // output;
        VertexToSingleLabelClusterMap &vertex_to_single_label_cluster_map =
                result.vertex_to_single_label_cluster_map; // output
        using LabelToVertexMap =
                std::unordered_map<LabelType, vertex_descriptor>;
        LabelToVertexMap &label_to_vertex_representing_cluster_map =
                result.label_to_vertex_representing_cluster_map;
        // Structure to convert vertex_descriptors from m_vertex_to_cluster_map
        // to labels.
        using VertexToLabelMap =
                std::unordered_map<vertex_descriptor, LabelType>;
        VertexToLabelMap vertex_to_label_map;
        // Used to link a label to the vertex descriptor representing the
        // cluster
        for (const auto &key_value : m_vertex_to_cluster_map) {
            const auto &cluster_vertices = key_value.second;
            const auto cluster_vertices_size = std::size(cluster_vertices);
            // if (m_verbose) {
            //     std::cout << "cluster_size: " << cluster_vertices_size
            //               << std::endl;
            // }
            if (cluster_vertices_size == 0) {
                throw std::runtime_error(
                        "Impossible that cluster_vertices is empty");
            }
            // Ignore "clusters" with only one vertex for this cleaned map.
            if (cluster_vertices_size == 1)
                continue;

            // The first element of set is the smaller vertex descriptor of the
            // cluster
            const auto vertex_descriptor_representing_cluster =
                    *cluster_vertices.begin();
            const auto &label = vertex_descriptor_representing_cluster;

            vertex_to_single_label_cluster_map.emplace(key_value.first, label);

            // Also add to label to vertex map
            const bool label_already_exists =
                    (label_to_vertex_representing_cluster_map.find(label) !=
                     label_to_vertex_representing_cluster_map.end())
                            ? true
                            : false;
            if (!label_already_exists) {
                label_to_vertex_representing_cluster_map.emplace(
                        label, vertex_descriptor_representing_cluster);
            }
        }
        return result;
    };

    SingleLabelMaps single_label_maps_to_centroid(
            const SingleLabelMaps &input_single_label_maps,
            const SpatialGraph &input_sg) {
        SingleLabelMaps output = input_single_label_maps;
        auto &vertex_to_centroid_cluster_map =
                output.vertex_to_single_label_cluster_map;
        auto &label_to_centroids_map =
                output.label_to_vertex_representing_cluster_map;

        for (auto key_value : label_to_centroids_map) {
            auto &vertex_descriptor_representing_cluster = key_value.second;
            const auto cluster_vertices = m_vertex_to_cluster_map.at(
                    vertex_descriptor_representing_cluster);
            auto centroid_vertex =
                    get_vertex_closer_to_centroid(cluster_vertices, input_sg);
            vertex_descriptor_representing_cluster = centroid_vertex;
        }
        // Now modify the cluster_label_map with the new vertex representing
        // the cluster
        for (auto vertex_label : vertex_to_centroid_cluster_map) {
            auto &label = vertex_label.second;
            label = label_to_centroids_map.at(
                    label); // convert label to centroid.
        }
        return output;
    }

};

} // end namespace SG
#endif
