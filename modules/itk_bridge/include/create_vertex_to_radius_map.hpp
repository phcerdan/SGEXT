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

#ifndef SG_CREATE_VERTEX_TO_RADIUS_MAP_HPP
#define SG_CREATE_VERTEX_TO_RADIUS_MAP_HPP

#include "image_types.hpp" // for image types
#include "spatial_graph.hpp"
#include "transform_to_physical_point.hpp" // for physical_space_array_to_index_array

#include <boost/graph/filtered_graph.hpp> // for create_vertex_to_radius_map
#include <boost/core/ignore_unused.hpp>
#include <unordered_map>

namespace SG {

using VertexToRadiusMap =
        std::unordered_map<typename GraphType::vertex_descriptor, double>;

/**
 * Create a vertex to local radius map from a distance map and a graph.
 *
 * @tparam TGraph to work with filter_graphs as well.
 *
 * @param distance_map_image obtained from a binary image @sa
 * create_distance_map_function
 * @param input_graph input spatial graph to get the vertices/nodes
 * @param spatial_nodes_position_are_in_physical_space flag to check if
 *  position of nodes were already converted to physical space, or are still in index space.
 *  false indicates that positions are in index space. See @sa transform_to_physical_point
 * @param verbose extra information at execution
 *
 * @return vertex to local radius map using the distance map
 */
template<typename TGraph>
VertexToRadiusMap create_vertex_to_radius_map(
        const typename FloatImageType::Pointer &distance_map_image,
        const TGraph &input_graph,
        const bool spatial_nodes_position_are_in_physical_space = false,
        const bool verbose = false)
{
    boost::ignore_unused(verbose);
    VertexToRadiusMap vertex_to_local_radius_map;
    // Iterate over all nodes
    using vertex_iterator = typename boost::graph_traits<TGraph>::vertex_iterator;
    vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(input_graph);
    for (; vi != vi_end; vi++) {
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

// explicit instantiation in create_vertex_to_radius_map.cpp
extern template VertexToRadiusMap create_vertex_to_radius_map<GraphType>(
        const typename FloatImageType::Pointer &distance_map_image,
        const GraphType &input_graph,
        const bool spatial_nodes_position_are_in_physical_space,
        const bool /*verbose*/);

} // end namespace SG
#endif
