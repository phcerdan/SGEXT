/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef TRANSFORM_TO_PHYSICAL_POINT_HPP
#define TRANSFORM_TO_PHYSICAL_POINT_HPP

#include "spatial_edge.hpp"  // for SG::PointType
#include "spatial_graph.hpp" // for SG::GraphAL

namespace SG {

/**
 *
 *      D0, D1, D2
 * D =  D3, D4, D5
 *      D6, D7, D8
 *
 *
 */
using DirectionMatrixType = std::array<double, 9>;

/**
 *
 * Prefer using an ITK image (optimized)
 *
 * ITK transform index to physical point. See:
 * https://discourse.itk.org/t/solved-transformindextophysicalpoint-manually/1031/10
 * @param input_array
 * @param origin
 * @param spacing
 * @param direction
 *
 * @return physical point
 */
inline SG::PointType
index_array_to_physical_space_array(const SG::PointType &input_array,
                                    const SG::PointType &origin,
                                    const SG::PointType &spacing,
                                    const SG::DirectionMatrixType &direction) {
    // clang-format off
    const SG::PointType physical_array({
        origin[0] +
        (direction[0] * spacing[0]) * input_array[0] +
        (direction[1] * spacing[1]) * input_array[1] +
        (direction[2] * spacing[2]) * input_array[2],

        origin[1] +
        (direction[3] * spacing[0]) * input_array[0] +
        (direction[4] * spacing[1]) * input_array[1] +
        (direction[5] * spacing[2]) * input_array[2],

        origin[2] +
        (direction[6] * spacing[0]) * input_array[0] +
        (direction[7] * spacing[1]) * input_array[1] +
        (direction[8] * spacing[2]) * input_array[2]
    });
    // clang-format on
    return physical_array;
}

inline void
transform_graph_to_physical_space(SG::GraphAL &sg,
                                  const SG::PointType &origin,
                                  const SG::PointType &spacing,
                                  const SG::DirectionMatrixType &direction) {
    // Loop over all nodes and transform pos.
    auto verts = boost::vertices(sg);
    for (auto &&vi = verts.first; vi != verts.second; ++vi) {
        sg[*vi].pos = index_array_to_physical_space_array(
                sg[*vi].pos, origin, spacing, direction);
    }
    // Loop over all edges and transform edge_points
    auto edges = boost::edges(sg);
    for (auto ei = edges.first; ei != edges.second; ++ei) {
        for (auto &&ep : sg[*ei].edge_points) {
            ep = index_array_to_physical_space_array(
                    ep, origin, spacing, direction);
        }
    }
};

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
        sg[*vi].pos =
                index_array_to_physical_space_array(sg[*vi].pos, itk_image);
    }
    // Loop over all edges and transform edge_points
    auto edges = boost::edges(sg);
    for (auto ei = edges.first; ei != edges.second; ++ei) {
        for (auto &&ep : sg[*ei].edge_points) {
            ep = index_array_to_physical_space_array(ep, itk_image);
        }
    }
};

template <typename TImage>
void transform_graph_to_index_space(SG::GraphAL &sg, const TImage *itk_image) {
    // Loop over all nodes and transform pos.
    auto verts = boost::vertices(sg);
    for (auto &&vi = verts.first; vi != verts.second; ++vi) {
        sg[*vi].pos =
                physical_space_array_to_index_array(sg[*vi].pos, itk_image);
    }
    // Loop over all edges and transform edge_points
    auto edges = boost::edges(sg);
    for (auto ei = edges.first; ei != edges.second; ++ei) {
        for (auto &&ep : sg[*ei].edge_points) {
            ep = physical_space_array_to_index_array(ep, itk_image);
        }
    }
};

} // namespace SG
#endif
