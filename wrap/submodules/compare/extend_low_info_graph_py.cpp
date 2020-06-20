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

#include "extend_low_info_graph.hpp"
#include <vtkSmartPointer.h>

namespace py = pybind11;
using namespace SG;

void init_extend_low_info_graph(py::module &m) {
    m.def(
            "extend_low_info_graph",
            [](const std::vector<std::reference_wrapper<const GraphType>>
                       &graphs,
               std::unordered_map<vtkIdType, std::vector<graph_descriptor>>
                       &idMap,
               vtkSmartPointer<vtkOctreePointLocator> &octree, double radius,
               bool verbose) {
                return extend_low_info_graph_via_dfs(
                        graphs, idMap, octree.Get(), radius, verbose);
            },
            R"(
Visit the tree using the extend_low_info_graph_visitor

The original idea of this visitor is to create a result (m_result_sg) graph
with the same amount of nodes and edges than the low info graph, but where
info from high info graphs is gathered to move the end-points, and extend the
edges.

The resulting m_result_sg will be the first step to build the final result,
where high information is integrated, but
the graph should be still precise with high
confidence.

The result will be the scaffold where more branches from high info graphs will
be added.

Parameters:
----------
graphs: [GraphType]
id_map: dict(vtkIdType, [graph_descriptor])
octree: vtkOctreePointLocator
radius: double
verbose: bool
            )",
            py::arg("graphs"), py::arg("id_map"), py::arg("octree"),
            py::arg("radius"), py::arg("verbose") = false);
}
