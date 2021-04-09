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

#ifndef SGEXT_IMAGE_TYPES_HPP
#define SGEXT_IMAGE_TYPES_HPP

#include "itkImage.h"

namespace SG {
    constexpr unsigned int BinaryImageDimension = 3;
    using BinaryImagePixelType = unsigned char;
    using BinaryImageType = itk::Image<BinaryImagePixelType, BinaryImageDimension>;

    constexpr unsigned int FloatImageDimension = 3;
    using FloatImagePixelType = float;
    using FloatImageType = itk::Image<FloatImagePixelType, FloatImageDimension>;
} // end ns
#endif
