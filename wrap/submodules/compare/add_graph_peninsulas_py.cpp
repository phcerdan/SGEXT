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
