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
        boost::depth_first_visit(input_sg, *vi, vis, propColorMap);
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
