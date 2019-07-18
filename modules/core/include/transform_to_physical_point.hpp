/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef TRANSFORM_TO_PHYSICAL_POINT_HPP
#define TRANSFORM_TO_PHYSICAL_POINT_HPP

#include "spatial_edge.hpp"  // for SG::PointType
#include "spatial_graph.hpp" // for SG::GraphAL

namespace SG {

template <typename TImage>
SG::PointType
index_array_to_physical_space_array(const SG::PointType &input_array,
                                    const TImage *itk_image) {
    constexpr auto dim = TImage::ImageDimension;
    if (input_array.size() != dim)
        throw std::runtime_error(
                "index_array_to_physical_space_array, image dimension" +
                std::to_string(dim) + " doesn't fit with array size " +
                std::to_string(input_array.size()));

    using ITKIndexType = typename TImage::IndexType;
    using ITKPointType = typename TImage::PointType;

    ITKIndexType input_index;
    for (size_t i = 0; i < dim; i++) {
        input_index[i] = input_array[i];
    }

    ITKPointType physical_point;
    itk_image->TransformIndexToPhysicalPoint(input_index, physical_point);

    SG::PointType ret;
    for (size_t i = 0; i < dim; i++) {
        ret[i] = physical_point[i];
    }
    return ret;
};

template <typename TImage>
SG::PointType
physical_space_array_to_index_array(const SG::PointType &input_array,
                                    const TImage *itk_image) {
    constexpr auto dim = TImage::ImageDimension;
    if (input_array.size() != dim)
        throw std::runtime_error(
                "index_array_to_physical_space_array, image dimension" +
                std::to_string(dim) + " doesn't fit with array size " +
                std::to_string(input_array.size()));

    using ITKIndexType = typename TImage::IndexType;
    using ITKPointType = typename TImage::PointType;

    ITKPointType input_physical_point;
    for (size_t i = 0; i < dim; i++) {
        input_physical_point[i] = input_array[i];
    }

    ITKIndexType index_point;
    itk_image->TransformPhysicalPointToIndex(input_physical_point, index_point);

    SG::PointType ret;
    for (size_t i = 0; i < dim; i++) {
        ret[i] = index_point[i];
    }
    return ret;
};

template <typename TImage>
void transform_graph_to_physical_space(SG::GraphAL &sg,
                                       const TImage *itk_image) {
    // Loop over all nodes and transform pos.
    auto verts = boost::vertices(sg);
    for (auto &&vi = verts.first; vi != verts.second; ++vi) {
        sg[*vi].pos = std::move(
                index_array_to_physical_space_array(sg[*vi].pos, itk_image));
    }
    // Loop over all edges and transform edge_points
    auto edges = boost::edges(sg);
    for (auto ei = edges.first; ei != edges.second; ++ei) {
        for (auto &&ep : sg[*ei].edge_points) {
            ep = std::move(index_array_to_physical_space_array(ep, itk_image));
        }
    }
};

template <typename TImage>
void transform_graph_to_index_space(SG::GraphAL &sg, const TImage *itk_image) {
    // Loop over all nodes and transform pos.
    auto verts = boost::vertices(sg);
    for (auto &&vi = verts.first; vi != verts.second; ++vi) {
        sg[*vi].pos = std::move(
                physical_space_array_to_index_array(sg[*vi].pos, itk_image));
    }
    // Loop over all edges and transform edge_points
    auto edges = boost::edges(sg);
    for (auto ei = edges.first; ei != edges.second; ++ei) {
        for (auto &&ep : sg[*ei].edge_points) {
            ep = std::move(physical_space_array_to_index_array(ep, itk_image));
        }
    }
};

} // namespace SG
#endif
