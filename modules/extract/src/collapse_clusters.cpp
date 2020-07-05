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

#include "collapse_clusters.hpp"
#include "collapse_clusters_visitor.hpp"
#include "filter_spatial_graph.hpp" // for filter_component_graphs
#include "hash_edge_descriptor.hpp"

#include <boost/graph/connected_components.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>

namespace SG {
GraphType
collapse_clusters(const GraphType &input_sg,
                  const std::unordered_map<GraphType::vertex_descriptor,
                                           GraphType::vertex_descriptor>
                          &vertex_to_single_label_cluster_map,
                  bool verbose) {
    // First, get all out_edges from clusters.
    GraphType collapsed_graph;
    using vertex_descriptor = boost::graph_traits<GraphType>::vertex_descriptor;
    using edge_descriptor = boost::graph_traits<GraphType>::edge_descriptor;

    using ColorMap = std::map<vertex_descriptor, boost::default_color_type>;
    ColorMap colorMap;
    boost::associative_property_map<ColorMap> propColorMap(colorMap);

    using VertexMap = std::unordered_map<vertex_descriptor, vertex_descriptor>;
    VertexMap vertex_map;

    using EdgeAddedMap =
            std::unordered_map<edge_descriptor, bool, SG::edge_hash<GraphType>>;
    EdgeAddedMap edge_added_map;

    using CollapseClustersGraphVisitorType =
            CollapseClustersGraphVisitor<GraphType, VertexMap, EdgeAddedMap>;

    CollapseClustersGraphVisitorType vis(
            collapsed_graph, vertex_map, edge_added_map,
            vertex_to_single_label_cluster_map, verbose);

    // Run the visitor for each component of the graph
    auto filtered_component_graphs = filter_component_graphs(input_sg);
    for (const auto &component_graph : filtered_component_graphs) {
        using comp_vertex_iterator =
                boost::graph_traits<ComponentGraphType>::vertex_iterator;
        comp_vertex_iterator vi, vi_end;
        std::tie(vi, vi_end) = boost::vertices(component_graph);
        boost::depth_first_visit(input_sg, *vi, vis, propColorMap);
    }

    return collapsed_graph;
}

std::unordered_map<GraphType::vertex_descriptor, GraphType::vertex_descriptor>
trim_vertex_to_single_label_map(
        const std::vector<GraphType::vertex_descriptor> &cluster_labels,
        const std::unordered_map<GraphType::vertex_descriptor,
                                 GraphType::vertex_descriptor>
                &vertex_to_single_label_cluster_map) {
    // Trim the vertex_to_single_label_cluster_map to only keep the
    // specified cluster_labels
    std::unordered_map<GraphType::vertex_descriptor,
                       GraphType::vertex_descriptor>
            trimmed_vertex_to_single_label_cluster_map;
    for (const auto &vertex_label_pair : vertex_to_single_label_cluster_map) {
        const auto &vertex = vertex_label_pair.first;
        const auto &label = vertex_label_pair.second;
        const bool label_is_specified =
                (std::find(std::cbegin(cluster_labels),
                           std::cend(cluster_labels),
                           label) != std::cend(cluster_labels))
                        ? true
                        : false;
        if (label_is_specified) {
            trimmed_vertex_to_single_label_cluster_map.emplace(vertex, label);
        }
    }
    return trimmed_vertex_to_single_label_cluster_map;
}

GraphType collapse_specific_clusters(
        const std::vector<GraphType::vertex_descriptor> &cluster_labels,
        const GraphType &input_sg,
        const std::unordered_map<GraphType::vertex_descriptor,
                                 GraphType::vertex_descriptor>
                &vertex_to_single_label_cluster_map,
        bool verbose) {
    const auto trimmed_vertex_to_single_label_cluster_map =
            trim_vertex_to_single_label_map(cluster_labels,
                                            vertex_to_single_label_cluster_map);
    return collapse_clusters(
            input_sg, trimmed_vertex_to_single_label_cluster_map, verbose);
}
} // end namespace SG
