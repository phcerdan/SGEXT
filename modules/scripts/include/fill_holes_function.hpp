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
