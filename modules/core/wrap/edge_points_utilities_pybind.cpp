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

#include "edge_points_utilities.hpp"
#include "sgcore_common.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace SG;

void init_edge_points_utilities(py::module &m) {
    m.def("edge_points_length", &edge_points_length);
    m.def("contour_length", &contour_length);
    m.def("insert_unique_edge_point_with_distance_order",
          [](SpatialEdge::PointContainer &edge_points,
             const SpatialEdge::PointType &new_point)
                  -> SpatialEdge::PointContainer {
              insert_unique_edge_point_with_distance_order(edge_points,
                                                           new_point);
              return edge_points;
          });
}
