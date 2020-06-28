/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "collapse_clusters.hpp"
#include "collapse_clusters_visitor.hpp"
#include "filter_spatial_graph.hpp" // for filter_component_graphs

#include <boost/graph/connected_components.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>

namespace SG {
GraphType collapse_clusters(const GraphType &input_sg,
                            std::unordered_map<GraphType::vertex_descriptor,
                            GraphType::vertex_descriptor> &vertex_to_single_label_cluster_map,
                            bool verbose) {
    // First, get all out_edges from clusters.
    GraphType collapsed_graph;
    using vertex_descriptor = boost::graph_traits<GraphType>::vertex_descriptor;

    using ColorMap = std::map<vertex_descriptor, boost::default_color_type>;
    ColorMap colorMap;
    boost::associative_property_map<ColorMap> propColorMap(colorMap);

    using VertexMap = std::unordered_map<vertex_descriptor, vertex_descriptor>;
    VertexMap vertex_map;

    using CollapseClustersGraphVisitorType =
            CollapseClustersGraphVisitor<GraphType, VertexMap, ColorMap>;
    CollapseClustersGraphVisitorType vis(collapsed_graph, colorMap, vertex_map,
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
} // end namespace SG
