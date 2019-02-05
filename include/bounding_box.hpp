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

#ifndef BOUNDING_BOX_HPP
#define BOUNDING_BOX_HPP

#include "common_types.hpp" // PointType typedef

namespace SG {

struct BoundingBox {
  PointType ini = {{0.0, 0.0, 0.0}};
  PointType end = {{1.0, 1.0, 1.0}};

  BoundingBox() = default;
  BoundingBox(const PointType & input_ini, const PointType & input_end) :
    ini(input_ini), end(input_end) {};
  BoundingBox(const PointType & center, const std::array<size_t, 3> & radius)
  {
    for (unsigned int i = 0; i < 3; ++i) {
      ini[i] = center[i] - radius[i];
      end[i] = center[i] + radius[i];
    }
  };
  BoundingBox(const PointType &center, size_t radius) :
    BoundingBox(center, std::array<size_t, 3>{{radius, radius, radius}}) {};

  PointType GetSize() const {
    return {{ std::abs(end[0] - ini[0]), std::abs(end[1] - ini[1]) , std::abs(end[2] - ini[2])}};
  }
  PointType GetRadius() const {
    auto size = this->GetSize();
    return {{ size[0] / 2.0, size[1] / 2.0, size[2] / 2.0 }};
  }
  PointType GetCenter() const {
    auto radius = this->GetRadius();
    return {{ ini[0] + radius[0], ini[1] + radius[1], ini[2] + radius[2] }};
  }
  bool is_point_inside(const PointType & input_point) const {
  return
    input_point[0] >= ini[0] && input_point[0] <= end[0] &&
    input_point[1] >= ini[1] && input_point[1] <= end[1] &&
    input_point[2] >= ini[2] && input_point[2] <= end[2];
  }
};

bool is_inside(const PointType & input, const BoundingBox & box) {
  return box.is_point_inside(input);
};

} // end ns SG

#endif
