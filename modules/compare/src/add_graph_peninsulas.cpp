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

#include "add_graph_peninsulas.hpp"
#include "add_graph_peninsulas_visitor.hpp"
#include "filter_spatial_graph.hpp"
#include "graph_points_locator.hpp"
#include "spatial_graph_difference.hpp"
#include "split_edge.hpp"
#include <boost/graph/connected_components.hpp>
#include <tuple> // For std::tie
#include <vtkIdList.h>
#include <vtkPolyData.h>

namespace SG {

AddGraphPeninsulasResult add_graph_peninsulas(
        const std::vector<std::reference_wrapper<const GraphType>> &graphs,
        size_t extended_graph_index,
        size_t high_info_graph_index,
        vtkPointLocator *mergePoints,
        std::unordered_map<vtkIdType, std::vector<graph_descriptor>> &idMap,
        double radius_touch,
        bool verbose) {

    // Get the graph difference
    auto g_diff = spatial_graph_difference(graphs[high_info_graph_index],
                                           graphs[extended_graph_index],
                                           radius_touch, verbose);

    // Add the graph_descriptors of gdiff to idMap
    auto g_diff_points_map_pair = SG::get_vtk_points_from_graph(g_diff);
    SG::append_new_graph_points(g_diff_points_map_pair.first,
                                g_diff_points_map_pair.second, mergePoints,
                                idMap);

    // Rebuild the octree
    auto octree = SG::build_octree_locator(mergePoints->GetPoints());

    const size_t diff_graph_index = graphs.size();

    // Compute the number of components
    std::unordered_map<GraphType::vertex_descriptor, int> components_map;
    size_t num_of_components = boost::connected_components(
            g_diff, boost::make_assoc_property_map(components_map));

    std::vector<size_t> touch_extended_graph_count(num_of_components);
    std::vector<std::vector<IdWithGraphDescriptor>> touching_descriptors(
            num_of_components);
    for (const auto &comp : components_map) {
        const auto &vertex_d = comp.first;
        const auto &component_index = comp.second;
        auto pos = g_diff[vertex_d].pos;
        auto closeIdList = graph_closest_points_by_radius_locator(pos, octree,
                                                                  radius_touch);
        auto closest_descriptors =
                closest_existing_descriptors_by_graph(closeIdList, idMap);
        auto &extended_graph_id_with_graph_descriptor =
                closest_descriptors[extended_graph_index];
        // vertex or edge exist.
        // TODO. Does the logic change if is_vertex or is_edge?
        // if is_edge, the resulting graph will add a node and split the
        // existing edge. if is_vertex, no vertex needs to be added.
        if (!extended_graph_id_with_graph_descriptor.exist) {
            touch_extended_graph_count[component_index]++;
            // Only save descriptors for the first touching descriptor
            // There is only one for peninsula components (memory optimization)
            if (touch_extended_graph_count[component_index] == 1) {
                touching_descriptors[component_index] = closest_descriptors;
            }
        }
    }
    // Add components with zero (islands) or one touching points (peninsulas)
    // Avoid adding multi-touching
    // Add islands and peninsulas to the extended graph
    GraphType result_sg = graphs[extended_graph_index];
    // XXX: How we add these components?
    // - filtered_graph to get iterators to these components.
    auto filtered_component_graphs =
            filter_component_graphs(g_diff, num_of_components, components_map);
    for (unsigned int comp_graph_index = 0;
         comp_graph_index < num_of_components; ++comp_graph_index) {
        // Only analyze isolated components and peninsulas
        if (touch_extended_graph_count[comp_graph_index] > 1) {
            continue;
        }

        // Locate the touching point
        GraphType::vertex_descriptor already_added_vertex;
        SplitEdge<GraphType> splitEdge;
        if (touching_descriptors[comp_graph_index].size() == 1) {
            const auto &touching_extended_descriptor =
                    touching_descriptors[comp_graph_index][extended_graph_index]
                            .descriptor;
            const auto &touching_diff_descriptor =
                    touching_descriptors[comp_graph_index][diff_graph_index]
                            .descriptor;
            if (touching_extended_descriptor.is_vertex) {
                // if the touching point is a vertex in the low info graph
                // store it to avoid adding it twice when iterating the
                // component graph
                // XXX: Here we assume that the touching point is always a
                // vertex in the diff graph This is correct due to the
                // spatial_graph_difference implementation.
                assert(touching_diff_descriptor.is_vertex == true);
                already_added_vertex = touching_diff_descriptor.vertex_d;
            } else if (touching_extended_descriptor.is_edge) {
                // if the touching point is an edge in the low graph, we have to
                // split that edge in the result graph when adding the vertex
                splitEdge = SG::split_edge(
                        touching_extended_descriptor.edge_points_index,
                        touching_extended_descriptor.edge_d, result_sg);
                already_added_vertex = splitEdge.vertex_descriptor_added;
            } else {
                throw std::runtime_error(
                        "Impossible, extended_graph descriptor has "
                        "to exist at the touch point");
            }
        }

        // Add/Copy the component graph into the resulting grap
        {
            // -- Manually (no use of copy graph) --
            // Iterate over edges of component graph,
            // add source/target nodes and edge into the result graph
            // Except if source/target is a touching node, then add no edge
            // but use the existing node in result graph
            // Information about that vertex in: already_added_vertex
            auto &component_graph = filtered_component_graphs[comp_graph_index];
            ComponentGraphType::edge_iterator ei, ei_end;
            std::tie(ei, ei_end) = boost::edges(component_graph);
            for (; ei != ei_end; ++ei) {
                auto source = boost::source(*ei, component_graph);
                auto target = boost::target(*ei, component_graph);
                auto &source_spatial_node = component_graph[source];
                auto &target_spatial_node = component_graph[target];
                auto &component_edge = component_graph[*ei];
                const auto &touching_diff_descriptor =
                        touching_descriptors[comp_graph_index][diff_graph_index]
                                .descriptor;
                // Note that the touching point of the diff is always a vertex,
                // because implementation of spatial_graph_difference
                const auto &diff_vertex_descriptor =
                        touching_diff_descriptor.vertex_d;
                GraphType::vertex_descriptor result_source;
                GraphType::vertex_descriptor result_target;
                if (source != diff_vertex_descriptor) {
                    result_source =
                            boost::add_vertex(source_spatial_node, result_sg);
                } else {
                    result_source = already_added_vertex;
                }
                if (target != diff_vertex_descriptor) {
                    result_target =
                            boost::add_vertex(target_spatial_node, result_sg);
                } else {
                    result_target = already_added_vertex;
                }
                boost::add_edge(result_source, result_target, component_edge,
                                result_sg);
            }
        } // end copying
    }

    return AddGraphPeninsulasResult{result_sg, octree};
};

} // end namespace SG
