/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SGITK_COMMON_PYBIND_HPP
#define SGITK_COMMON_PYBIND_HPP

#include <itkImage.h>

namespace SG {
using IUC3 = itk::Image<unsigned char, 3>;
using IUC3P = typename IUC3::Pointer;
using IF3 = itk::Image<float, 3>;
using IF3P = typename IF3::Pointer;
}

#endif
