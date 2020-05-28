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
    m.def("edge_points_length", &edge_points_length, R"(
Computes the length of the edge without taking into account the node points.
It requires the edge_points to be contiguous/ordered.
    )");
    m.def("contour_length", &contour_length, R"(
Computes the contour_length of an edge between two nodes.
It takes into account the nodes positions.
It requires the edge_points to be contiguous/ordered.
    )");
    m.def(
            "insert_unique_edge_point_with_distance_order",
            [](SpatialEdge::PointContainer &edge_points,
               const SpatialEdge::PointType &new_point)
                    -> SpatialEdge::PointContainer {
                insert_unique_edge_point_with_distance_order(edge_points,
                                                             new_point);
                return edge_points;
            },
            R"(
Insert point in the input container.
The input container is a list of points ordered by connectvity, consecutive
points in the container are connected.
This computes the distance of the new_point with all the existing
edge_points.

Parameters:
----------
edge_points: [Points]
    Point container with existing edge_points.
    This container is not modified, for that, use the returned value.
    new_edge_points = insert_unique_edge_point_with_distance_order(edge_points, new_point)
new_point: Point
    The point to insert in the container.
          )",
            py::arg("edge_points"), py::arg("new_point"));

    m.def("check_edge_points_are_contiguous", &check_edge_points_are_contiguous,
          R"(
Check edge_points are contiguous.
It requires the edge_points to be in index space (i.e spacing = [1, 1, 1])
If compiled in Debug mode it prints the failed points to std::cerr.
)",
            py::arg("edge_points"));
}
