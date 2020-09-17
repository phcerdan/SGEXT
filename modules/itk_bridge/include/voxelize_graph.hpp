/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_VOXELIZE_GRAPH_HPP
#define SG_VOXELIZE_GRAPH_HPP

#include "image_types.hpp"
#include "spatial_graph.hpp"
#include "spatial_graph_io.hpp" // for vertex_to_label_map_t
#include <functional>
#include <unordered_map>

namespace SG {

using edge_label_function_t = std::function<size_t(const size_t &source_label,
                                                   const size_t &target_label)>;

/**
 * Helper function to get an ITK image index from a graph position.
 *
 * If the graph_positions_are_in_physical_space applies a
 * TransformPhysicalPointToIndex using the refence_image.
 *
 * @tparam ImageType BinaryImageType or FloatImageType
 * @param input_point input point from a spatial_node.pos or
 * spatial_edge.edge_points[ep_index].
 * @param reference_image reference ITK image
 * @param graph_positions_are_in_physical_space true if the positions are
 *  in physical space @sa transform_graph_to_physical_space.
 *
 * @return
 */
template <typename ImageType>
typename ImageType::IndexType graph_position_to_image_index(
        const ArrayUtilities::Array3D &input_point,
        const typename ImageType::Pointer &reference_image,
        const bool graph_positions_are_in_physical_space = true) {
    if (graph_positions_are_in_physical_space) {
        typename ImageType::PointType itk_point;
        typename ImageType::IndexType itk_index;
        for (size_t i = 0; i < ImageType::ImageDimension; ++i) {
            itk_point[i] = input_point[i];
        }
        reference_image->TransformPhysicalPointToIndex(itk_point, itk_index);
        return itk_index;
    } else {
        typename ImageType::IndexType itk_index;
        for (size_t i = 0; i < ImageType::ImageDimension; ++i) {
            itk_index[i] = input_point[i];
        }
        return itk_index;
    }
}

/**
 * Create an edge_to_label_map (needed for @ref voxelize_graph) from a
 * vertex_to_label_map and a functor of type func(source_label, target_label) ->
 * edge_label.
 *
 * If a existing node in the graph is not found in the input vertex_to_label_map
 * The functor won't be applied on the edges having that node as source or
 * target, and that edge won't be emplaced on the resulting edge map.
 *
 * @param graph input spatial graph
 * @param vertex_to_label_map
 * @param edge_label_func
 *
 * @return
 */
edge_to_label_map_t create_edge_to_label_map_from_vertex_to_label_map(
        const GraphType &graph,
        const vertex_to_label_map_t &vertex_to_label_map,
        const edge_label_function_t &edge_label_func);

/**
 * Use @ref create_edge_to_label_map_from_vertex_to_label_map with the functor
 * std::max(source_label, target_label).
 * Each edge will have the label corresponding to the max label from
 * source_label, target_label.
 *
 * @param graph input spatial graph
 * @param vertex_to_label_map
 *
 * @return
 */
edge_to_label_map_t create_edge_to_label_map_from_vertex_to_label_map_using_max(
        const GraphType &graph,
        const vertex_to_label_map_t &vertex_to_label_map);

/**
 * Create an image from the input graph.
 * Uses a reference_image (for example, the thin image used to create the graph)
 * to set the size and metadata (origin, spacing, direction) of the output
 * image.
 *
 * It takes two label maps, one for vertices and other for edges. You can
 * create an edge_to_label_map using the helper functions
 * @ref create_edge_to_label_map_from_vertex_to_label_map
 * @ref create_edge_to_label_map_from_vertex_to_label_map_using_max
 *
 * @param graph input spatial graph
 * @param reference_image ITK binary image
 * @param vertex_to_label_map vertex_descriptor -> size_t
 * @param edge_to_label_map edge_descriptor -> size_t
 * @param graph_positions_are_in_physical_space
 *
 * @return
 */
BinaryImageType::Pointer
voxelize_graph(const GraphType &graph,
               const BinaryImageType::Pointer &reference_image,
               const vertex_to_label_map_t &vertex_to_label_map,
               const edge_to_label_map_t &edge_to_label_map,
               const bool &graph_positions_are_in_physical_space = true);
} // end namespace SG
#endif
