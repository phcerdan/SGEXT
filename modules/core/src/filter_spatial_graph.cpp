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

#include "filter_spatial_graph.hpp"
#include "spatial_edge.hpp"
#include "spatial_node.hpp"
#include <boost/graph/connected_components.hpp>
#include <boost/graph/copy.hpp>

namespace SG {

FilteredGraphType filter_by_bounding_box_no_copy(const BoundingBox &box,
                                                 const GraphType &g) {
    auto edge_lambda = [&](GraphType::edge_descriptor ed) -> bool {
        auto &edge_points = g[ed].edge_points;
        for (auto &point : edge_points) {
            if (is_inside(point, box)) {
                return true;
            }
        }
        return false;
    };
    auto vertex_lambda = [&](GraphType::vertex_descriptor vd) -> bool {
        const auto &pos = g[vd].pos;
        if (is_inside(pos, box)) {
            return true;
        } else { // iterate over the edges to check edge_points are inside or
                 // not
            const auto out_edges = boost::out_edges(vd, g);
            for (auto ei = out_edges.first; ei != out_edges.second; ++ei) {
                const auto &edge_points = g[*ei].edge_points;
                for (const auto &point : edge_points) {
                    if (is_inside(point, box)) {
                        // nodes_outside_with_edges_inside.push_back(pos);
                        return true;
                    }
                }
            }
        }
        return false;
    };

    return FilteredGraphType(g, edge_lambda, vertex_lambda);
};

GraphType filter_by_bounding_box(const BoundingBox &box, const GraphType &g) {
    auto filtered_view = filter_by_bounding_box_no_copy(box, g);

    GraphType out_filtered_graph;
    boost::copy_graph(filtered_view, out_filtered_graph);
    return out_filtered_graph;
};

FilteredGraphType
filter_by_sets_no_copy(const EdgeDescriptorUnorderedSet &remove_edges,
                       const VertexDescriptorUnorderedSet &remove_nodes,
                       const GraphType &g) {
    // WARNING: We would have to capture the sets by value if we want to copy
    // around the FilteredGraph if the input remove_ sets go out of scope.
    auto edge_lambda = [&remove_edges](GraphType::edge_descriptor ed) -> bool {
        return !static_cast<bool>(remove_edges.count(ed));
    };
    auto vertex_lambda =
            [&remove_nodes](GraphType::vertex_descriptor vd) -> bool {
        return !static_cast<bool>(remove_nodes.count(vd));
    };

    return FilteredGraphType(g, edge_lambda, vertex_lambda);
}

GraphType filter_by_sets(const EdgeDescriptorUnorderedSet &remove_edges,
                         const VertexDescriptorUnorderedSet &remove_nodes,
                         const GraphType &g) {
    auto filtered_view = filter_by_sets_no_copy(remove_edges, remove_nodes, g);
    // Copy the graph -- vertex and edge descriptors are invalidated...
    GraphType out_filtered_graph;
    boost::copy_graph(filtered_view, out_filtered_graph);
    return out_filtered_graph;
};

std::vector<ComponentGraphType> filter_component_graphs(
        const GraphType &inputGraph,
        const size_t num_of_components,
        const std::unordered_map<GraphType::vertex_descriptor, int>
                &components_map) {
    std::vector<ComponentGraphType> component_graphs;
    for (size_t comp_index = 0; comp_index < num_of_components; comp_index++)
        component_graphs.emplace_back(
                inputGraph,
                // edge_lambda
                [components_map, comp_index,
                 &inputGraph](GraphType::edge_descriptor e) {
                    return components_map.at(source(e, inputGraph)) ==
                                   comp_index ||
                           components_map.at(target(e, inputGraph)) ==
                                   comp_index;
                },
                // vertex_lambda
                [components_map, comp_index](GraphType::vertex_descriptor v) {
                    return components_map.at(v) == comp_index;
                });

    return component_graphs;
}

std::vector<ComponentGraphType>
filter_component_graphs(const GraphType &inputGraph) {
    std::unordered_map<GraphType::vertex_descriptor, int> components_map;
    auto num_of_components = boost::connected_components(
            inputGraph, boost::make_assoc_property_map(components_map));
    return filter_component_graphs(inputGraph, num_of_components,
                                   components_map);
}
} // namespace SG
