/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_CREATE_VERTEX_TO_RADIUS_MAP_HPP
#define SG_CREATE_VERTEX_TO_RADIUS_MAP_HPP

#include "image_types.hpp" // for image types
#include "spatial_graph.hpp"

#include <unordered_map>

namespace SG {

using VertexToRadiusMap =
        std::unordered_map<typename GraphType::vertex_descriptor, double>;

/**
 * Create a vertex to local radius map from a distance map and a graph.
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
VertexToRadiusMap create_vertex_to_radius_map(
        const typename FloatImageType::Pointer &distance_map_image,
        const GraphType &input_graph,
        const bool spatial_nodes_position_are_in_physical_space = false,
        const bool verbose = false);

} // end namespace SG
#endif
