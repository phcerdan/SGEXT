/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include "locate/sglocate_common.h" // define holder for vtkSmartPointer

#include "add_graph_peninsulas.hpp"

namespace py = pybind11;
using namespace SG;

void init_add_graph_peninsula(py::module &m) {
    py::class_<AddGraphPeninsulasResult>(m, "add_graph_peninsulas_result")
            .def_readwrite("graph", &AddGraphPeninsulasResult::graph)
            .def_readwrite("octree", &AddGraphPeninsulasResult::octree);

    /* *********************************************************************/

    m.def(
            "add_graph_peninsulas",
            [](const std::vector<std::reference_wrapper<const GraphType>>
                       &graphs,
               size_t extended_graph_index, size_t high_info_graph_index,
               vtkSmartPointer<vtkPointLocator> &mergePoints,
               std::unordered_map<vtkIdType, std::vector<graph_descriptor>>
                       &idMap,
               double radius_touch, bool verbose) {
                return add_graph_peninsulas(
                        graphs, extended_graph_index, high_info_graph_index,
                        mergePoints.Get(), idMap, radius_touch, verbose);
            },
            R"(
A peninsula is any subgraph from graphs[high_info_graph_index] only
contacting the result graph in one point.

Parameters:
----------
graphs: [GraphType]
high_info_graph_index: int
extended_graph_index: int
merge_points_locator: vtkPointLocator
id_map: dict(vtkIdType, [graph_descriptor])
radius_touch: double
verbose: bool
            )",
            py::arg("graphs"), py::arg("extended_graph_index"),
            py::arg("high_info_graph_index"), py::arg("merge_points_locator"),
            py::arg("id_map"), py::arg("radius_touch"),
            py::arg("verbose") = false);

    /* *********************************************************************/
}
