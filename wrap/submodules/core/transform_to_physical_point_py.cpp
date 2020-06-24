/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
