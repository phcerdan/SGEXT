/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FILL_HOLES_FUNCTION_HPP
#define FILL_HOLES_FUNCTION_HPP

#include "scripts_types.hpp"

namespace SG {

/**
 * Use itk::VotingBinaryIterativeHoleFillingImageFilter to fill holes in a
 * binary image.
 *
 * @param input binary image to with holes to fill
 * @param radius to define the neighborhood.
 * @param majority Majority is the number of pixels in the neighborhood
 * of an OFF pixel, to turn it into ON.
 * By default majority = 1, this means that an off pixel will be turned on
 * if in the neighborhood (set by radius) there are at least 50% + 1 pixels ON.
 * If radius = 1,1,1, neighborhood size will be 3x3 = 9 pixels.
 * If 5 pixels around an OFF pixel are ON, then it will be switched.
 * @param iterations number of iterations for
 * @param verbose extra info to cout
 *
 * @return  binary image
 */
BinaryImageType::Pointer fill_holes_voting_iterative_function(
        const BinaryImageType::Pointer & input,
        const int & radius = 1,
        const int & majority = 3,
        const size_t & iterations = 1000,
        bool verbose = false
        );
} // end ns SG
#endif
