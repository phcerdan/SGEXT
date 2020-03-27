/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SCRIPTS_TYPES_HPP
#define SCRIPTS_TYPES_HPP

#include "itkImage.h"

namespace SG {
    constexpr unsigned int BinaryImageDimension = 3;
    using BinaryImagePixelType = unsigned char;
    using BinaryImageType = itk::Image<BinaryImagePixelType, BinaryImageDimension>;
} // end ns
#endif
