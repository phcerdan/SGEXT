/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "create_vertex_to_radius_map.hpp"
#include "transform_to_physical_point.hpp" // for physical_space_array_to_index_array

namespace SG {

VertexToRadiusMap create_vertex_to_radius_map(
        const typename FloatImageType::Pointer &distance_map_image,
        const GraphType &input_graph,
        const bool spatial_nodes_position_are_in_physical_space,
        const bool verbose) {
    VertexToRadiusMap vertex_to_local_radius_map;
    // Iterate over all nodes
    for (auto [vi, vi_end] = boost::vertices(input_graph); vi != vi_end; vi++) {
        // Get the value of the distance map image associated to the position of
        // the node
        const auto spatial_node_position_index_space =
                spatial_nodes_position_are_in_physical_space
                        ? physical_space_array_to_index_array(
                                  input_graph[*vi].pos, distance_map_image.GetPointer())
                        : input_graph[*vi].pos;

        // Transform index_position to ITK IndexType
        typename FloatImageType::IndexType pixel_index;
        for (size_t i = 0; i < 3; i++) {
            pixel_index[i] = spatial_node_position_index_space[i];
        }

        const auto dmap_value = distance_map_image->GetPixel(pixel_index);
        // Populate the output map
        vertex_to_local_radius_map.emplace(*vi, static_cast<double>(dmap_value));
    }

    return vertex_to_local_radius_map;
}

} // end namespace SG
