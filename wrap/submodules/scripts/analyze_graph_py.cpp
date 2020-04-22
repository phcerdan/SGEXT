/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

#include "analyze_graph_function.hpp"

namespace py = pybind11;
using namespace SG;
void init_analyze_graph(py::module &m) {
    m.def("analyze_graph", &analyze_graph_function,
            "Get a spatial graph from a binary image",
            py::arg("input"),
            py::arg("removeExtraEdges") = true,
            py::arg("mergeThreeConnectedNodes") = true,
            py::arg("mergeFourConnectedNodes") = true,
            py::arg("mergeTwoThreeConnectedNodes") = true,
            py::arg("checkParallelEdges") = true,
            py::arg("transformToPhysicalPoints") = false,
            py::arg("spacing") = "",
            py::arg("output_filename_simple") = false,
            py::arg("exportReducedGraph_foldername") = "",
            py::arg("exportSerialized") = false,
            py::arg("exportVtu") = false,
            py::arg("exportVtuWithEdgePoints") = false,
            py::arg("exportGraphviz") = false,
            py::arg("exportData_foldername") = "",
            py::arg("ignoreAngleBetweenParallelEdges") = false,
            py::arg("ignoreEdgesToEndNodes") = false,
            py::arg("ignoreEdgesShorterThan") = 0,
            py::arg("verbose") = false,
            py::arg("visualize") = false
                );


}
