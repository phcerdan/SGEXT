/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CREATE_DISTANCE_MAP_FUNCTION_HPP
#define CREATE_DISTANCE_MAP_FUNCTION_HPP

#include <string>
#include "scripts_types.hpp"

namespace SG {
/**
 * Create a distance map image from a binary image
 * A distance map shows the distances of voxel to their nearest background voxel,
 * i.e distance to nearest border.
 *
 * @param input_img input binary image
 * @param verbose verbosity
 *
 * @return distance map image
 */
typename FloatImageType::Pointer create_distance_map_function(
        const typename BinaryImageType::Pointer & input_img,
        const bool verbose = false
        );

/**
 * @create_distance_map_function but using filenames as inputs/outputs
 *
 * @param input_img_file
 * @param outputFolder folder where output will be written
 * @param foreground the voxels representing the object are "white|black"
 * @param verbose verbosity
 */
typename FloatImageType::Pointer create_distance_map_function_io(
        const std::string & input_img_file,
        const std::string & outputFolder,
        const std::string & foreground = "white",
        const bool verbose = false
        );

} // end ns
#endif
