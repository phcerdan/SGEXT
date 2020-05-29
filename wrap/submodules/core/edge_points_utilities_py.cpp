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

#include "pybind11_common.h"
#include <pybind11/stl.h>

#include "edge_points_utilities.hpp"

namespace py = pybind11;
using namespace SG;

void init_edge_points_utilities(py::module &m) {
    m.def("ete_distance", &ete_distance, R"(
Computes the distance between the source and target nodes of the edge.
Ignores edge_points.
    )");
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
