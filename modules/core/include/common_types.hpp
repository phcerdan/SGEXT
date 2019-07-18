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

#ifndef SG_COMMON_TYPES_HPP
#define SG_COMMON_TYPES_HPP

#include "array_utilities.hpp"
#include <vector>

namespace SG {
using PointType = ArrayUtilities::Array3D;
using PointContainer = std::vector<PointType>;
/// Help reader to differentiate a point from a vector
using VectorType = ArrayUtilities::Array3D;
} // namespace SG
#endif
