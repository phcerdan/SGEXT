/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_RESAMPLE_IMAGE_FUNCTION_HPP
#define SG_RESAMPLE_IMAGE_FUNCTION_HPP

#include "scripts_types.hpp"

namespace SG {
/**
 * Resample binary image.
 *
 * If shrink_factor is greater than 1, it downsamples the image.
 * If shrink_factor is between (0, 1), it upsamples the image.
 *
 * Output image has the same origin, and the right spacing and size based on
 * shrink factor.
 *
 * @param input binary image
 * @param shrink_factor value greater than 0 to downsample or upsample the
 * image.
 * @param verbose extra info to std::cout
 *
 * @return resampled image
 */
BinaryImageType::Pointer
resample_image_function(const BinaryImageType::Pointer &input,
                         const double &shrink_factor,
                         bool verbose);
} // namespace SG

#endif
