/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "detect_clusters.hpp"
#include "detect_clusters_visitor.hpp"
#include "filter_spatial_graph.hpp" // for filter_component_graphs

#include <boost/graph/connected_components.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>

namespace SG {

std::unordered_map<GraphType::vertex_descriptor, GraphType::vertex_descriptor>
detect_clusters_with_radius(const GraphType &input_sg,
                            const double &cluster_radius,
                            bool use_cluster_centroid,
                            bool verbose) {

    using vertex_descriptor = boost::graph_traits<GraphType>::vertex_descriptor;
    using edge_descriptor = boost::graph_traits<GraphType>::edge_descriptor;

    using DetectClustersGraphVisitorType =
            DetectClustersGraphVisitor<GraphType>;
    auto cluster_edge_condition = [cluster_radius](edge_descriptor e,
                                                   const GraphType input_sg) {
        return DetectClustersGraphVisitorType::condition_edge_is_close(
                e, input_sg, cluster_radius);
    };
    typename DetectClustersGraphVisitorType::VertexToClusterMap
            vertex_to_cluster_map;

    DetectClustersGraphVisitorType vis(vertex_to_cluster_map,
                                       cluster_edge_condition, verbose);

    // For dfs/bfs
    using ColorMap = std::map<vertex_descriptor, boost::default_color_type>;
    ColorMap colorMap;
    boost::associative_property_map<ColorMap> propColorMap(colorMap);

    // Run the visitor for each component of the graph
    auto filtered_component_graphs = filter_component_graphs(input_sg);
    for (const auto &component_graph : filtered_component_graphs) {
        using comp_vertex_iterator =
                boost::graph_traits<ComponentGraphType>::vertex_iterator;
        comp_vertex_iterator vi, vi_end;
        std::tie(vi, vi_end) = boost::vertices(component_graph);
        boost::queue< vertex_descriptor > Q;  // buffer for bfs
        boost::breadth_first_visit(input_sg, *vi, Q, vis, propColorMap);
    }
    if(verbose) {
        std::cout << "vertex_to_cluster_map: " << std::endl;
        for(const auto & vertex_to_cluster_set : vis.m_vertex_to_cluster_map) {
            std::cout << vertex_to_cluster_set.first << " : [ ";
            for(const auto & vertex : vertex_to_cluster_set.second) {
                std::cout << vertex << ", ";
            }
            std::cout << " ]" << std::endl;
        }
    }
    const auto single_label_maps = vis.get_single_label_cluster_maps();
    if (use_cluster_centroid) {
        const auto centroid_single_label_maps =
                vis.single_label_maps_to_centroid(single_label_maps, input_sg);
        return centroid_single_label_maps.vertex_to_single_label_cluster_map;
    }
    return single_label_maps.vertex_to_single_label_cluster_map;
}

void assign_cluster_label_to_spatial_node_id(
        GraphType &input_sg,
        std::unordered_map<GraphType::vertex_descriptor, size_t>
                &cluster_label_map) {
    for (const auto vertex_cluster_id_pair : cluster_label_map) {
        const auto &vertex_descriptor = vertex_cluster_id_pair.first;
        const auto &cluster_id = vertex_cluster_id_pair.second;
        input_sg[vertex_descriptor].id = cluster_id;
    }
}

} // end namespace SG
