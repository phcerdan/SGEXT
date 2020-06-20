/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
