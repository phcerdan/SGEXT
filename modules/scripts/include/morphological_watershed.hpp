/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_MORPHOLOGICAL_WATERSHED_HPP
#define SG_MORPHOLOGICAL_WATERSHED_HPP

#include "scripts_types.hpp"

namespace SG {

/**
 * Performs a morphological watershed from markers to "refill" the original
 * binary image from a marker image that can have different labels,
 * marker_image is usually set to the output image of @ref voxelize_graph.
 *
 * @param original_binary_image original binary image
 * @param marker_image image with markers from for example @ref voxelize_graph
 *
 * @return original_binary_image but with labels propagated from the marker
 * image.
 */
BinaryImageType::Pointer
morphological_watershed(const BinaryImageType::Pointer &original_binary_image,
                        const BinaryImageType::Pointer &marker_image);

} // end namespace SG
#endif
