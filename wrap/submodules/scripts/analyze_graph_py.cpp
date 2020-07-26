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
    const std::string analyze_graph_docs = R"delimiter(
Returns a spatial graph from a binary image.

Parameters:
----------
input: str or SG::BinaryImageType
    Input thin image or input filename holding an skeletonized binary image.
    Use sgext.scripts.thin to generate it.

output_base_name: str
  When using as input an image, the base name for the outputs. Defaults to
  extracted_graph.

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
)delimiter";

    m.def("extract_graph_io", &analyze_graph_function_io,
            analyze_graph_docs.c_str(),
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

    m.def("extract_graph", &analyze_graph_function,
            analyze_graph_docs.c_str(),
        py::arg("input"),
        py::arg("output_base_name") = "extracted_graph",
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
