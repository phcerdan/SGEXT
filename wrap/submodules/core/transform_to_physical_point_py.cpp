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

#include "transform_to_physical_point.hpp"

namespace py = pybind11;
using namespace SG;

void init_transform_to_physical_point_without_itk(py::module &m) {
    m.def(
            "index_to_physical_point",
            [](const SG::PointType &input_array, const SG::PointType &origin,
               const SG::PointType &spacing,
               const SG::DirectionMatrixType &direction) {
                return index_array_to_physical_space_array(input_array, origin,
                                                           spacing, direction);
            },
            R"(
Apply an ITK-style TransformIndexToPhysicalPoint to the input index.
            )",
            py::arg("input_index"), py::arg("origin"), py::arg("spacing"),
            py::arg("direction"));

    /* ******************************************************************/

    m.def(
            "transform_graph_to_physical_space",
            [](const SG::GraphType &sg, const SG::PointType &origin,
               const SG::PointType &spacing,
               const SG::DirectionMatrixType &direction) {
                GraphType output_graph = sg;
                transform_graph_to_physical_space(output_graph, origin, spacing,
                                                  direction);
                return output_graph;
            },
            R"(
Apply an ITK-style TransformIndexToPhysicalPoint to all the points of the input graph.
            )",
            py::arg("graph"), py::arg("origin"), py::arg("spacing"),
            py::arg("direction"));
}
