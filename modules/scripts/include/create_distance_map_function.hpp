/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CREATE_DISTANCE_MAP_FUNCTION_HPP
#define CREATE_DISTANCE_MAP_FUNCTION_HPP

#include "image_types.hpp"
#include <string>

namespace SG {
/**
 * Create a distance map image from a binary image
 * A distance map shows the distances of voxel to their nearest background
 * voxel, i.e distance to nearest border.
 *
 * DGtal works with voxels of the same size, and it is the most precise.
 *
 * If using ITK, take into account that the distance map is eroded, not sure why
 * (tested with SignedDanielssonDistanceMapImageFilter,
 * SignedMaurerDistanceMapImageFilter and FastChamferDistanceImageFilter. The
 * good thing about ITK is that it takes into account spacing. However, if the
 * image Spacing is isotropic (the voxels are cubes) we can recover the distance
 * in physical points from the DGtal version.
 *
 * @param input_img input binary image
 * @param verbose verbosity
 *
 * @return distance map image
 */
typename FloatImageType::Pointer
create_distance_map_function(const typename BinaryImageType::Pointer &input_img,
                             const bool use_itk_approximate = false,
                             const bool verbose = false);
typename FloatImageType::Pointer create_distance_map_function_with_dgtal(
        const typename BinaryImageType::Pointer &input_img,
        const bool verbose = false);
typename FloatImageType::Pointer create_distance_map_function_with_itk(
        const typename BinaryImageType::Pointer &input_img,
        const bool verbose = false);

/**
 * @create_distance_map_function but using filenames as inputs/outputs
 *
 * @param input_img_file
 * @param outputFolder folder where output will be written
 * @param foreground the voxels representing the object are "white|black"
 * @param use_itk_approximate use a fast approximation for distance map.
 * Also ITK computes the distance map taking into account spacing of the image.
 * @param verbose verbosity
 */
typename FloatImageType::Pointer
create_distance_map_function_io(const std::string &input_img_file,
                                const std::string &outputFolder,
                                const std::string &foreground = "white",
                                const bool use_itk_approximate = false,
                                const bool verbose = false);

} // namespace SG
#endif
