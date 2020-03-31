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
