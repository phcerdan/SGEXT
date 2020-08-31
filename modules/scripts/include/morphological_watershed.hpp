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
