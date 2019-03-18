/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_COMMON_TYPES_HPP
#define SG_COMMON_TYPES_HPP

#include "array_utilities.hpp"
#include <vector>

namespace SG {
using PointType = ArrayUtilities::Array3D;
using PointContainer = std::vector<PointType>;
}  // namespace SG
#endif
