/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include <pybind11/stl.h>

#include "edge_points_utilities.hpp"

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
