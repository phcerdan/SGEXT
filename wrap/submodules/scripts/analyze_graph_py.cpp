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
            R"delimiter(
Returns a spatial graph from a binary image.

Parameters:
----------
input: str
    input filename holding an skeletonized binary image.
    Use sgext.scripts.thin to generate it.

removeExtraEdges: bool
    default: True
    We remove the "diagonal" edges when there are shorter ones.
    example:

         o                  o
         |\                 |
         o-o      ----->    o-o
         |/                 |
         o                  o

mergeThreeConnectedNodes: bool
    default: True
    Some nodes that are connected between them could be merged.
    Transforming three nodes with degree 3, to one node
    with degree 3 and adding the old nodes into the
    corresponding spatial edges.
    example:

        o                o
        |                |
        o                |
        |\___            |
        |    \           |
        o-----o---o  ->  o---------o
        |                |
        |                |
        o                o

mergeFourConnectedNodes: bool
    default: True
    Same concept than mergeThreeConnectedNodes
mergeTwoThreeConnectedNodes: bool
    default: True
    Same concept than mergeThreeConnectedNodes
checkParallelEdges:
    Check and print info about parallel edges in the
    graph. Use verbose option for output.
transformToPhysicalPoints:
    Positions in Spatial Graph takes into account metadata
    of the (origin,spacing,direction) itk image.
spacing: string
    default: ""
    Provide external spacing between voxels. Ignores
    metadata of itk image and apply it.
    example:
        "2.0 2.0 2.0"
output_filename_simple:
    Output filename does not contain the parameters used for this filter.
exportReducedGraph_foldername: string
    folder where store the result of the reduced graph
    after the merge steps.

exportSerialized: bool
    default: False
    export the resulting reduced graph in a serialized format,
    optimized to be used by other internal sgext algorithms.

exportVtu:
    default: False
    export the resulting reduced graph in the vtk format (.vtu)
    ready to be used in paraview, slicer, or other vtk-ready cosumers.
exportVtuWithEdgePoints:
    export the resulting reduced graph in the vtk format (.vtu)
    including the edge points.
    Useful to visualize the edges as tubes.

exportGraphviz: bool
    default: False
    Write graphviz (.dot) representing the reduced graph.
    Requires exportReducedGraph_foldername.

exportData_foldername: str
    folder where export data associated to the graph

ignoreAngleBetweenParallelEdges: bool
    used when exporting data.

ignoreEdgesToEndNodes: bool
    used when exporting data.

ignoreEdgesShorterThan: bool
    used when exporting data.

verbose: bool
    default: False
    extra information displayed during the algorithm.

visualize: bool
    default: False
    visualize outputs during the run
    )delimiter",
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
