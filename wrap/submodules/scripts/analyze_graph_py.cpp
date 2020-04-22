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
