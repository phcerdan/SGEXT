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
