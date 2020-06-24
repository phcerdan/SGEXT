/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include "sgitk_common_py.hpp"

#include "transform_to_physical_point.hpp"

namespace py = pybind11;

void init_transform_to_physical_point_with_itk(py::module &m) {
    const std::string transform_graph_to_physical_space_docs = R"(
Transform graph to physical space using an ITK image as reference for the metadata.
Uses Origin, Spacing and DirectionMatrix.

Assumes that the graph is currently in index space.

Parameters:
----------
graph: GraphType
reference_image: Image (binary or float)
)";
    /* ************** BinaryImage ****************************/
    m.def(
            "transform_graph_to_physical_space",
            [](const SG::GraphType &sg,
               SG::IUC3P input_image) -> SG::GraphType {
                SG::GraphType output_graph = sg;
                SG::transform_graph_to_physical_space(output_graph,
                                                      input_image.GetPointer());
                return output_graph;
            },
            transform_graph_to_physical_space_docs.c_str(), py::arg("graph"),
            py::arg("reference_image"));
    /* ************** FloatImage ****************************/
    m.def(
            "transform_graph_to_physical_space",
            [](const SG::GraphType &sg, SG::IF3P input_image) -> SG::GraphType {
                SG::GraphType output_graph = sg;
                SG::transform_graph_to_physical_space(output_graph,
                                                      input_image.GetPointer());
                return output_graph;
            },
            transform_graph_to_physical_space_docs.c_str(), py::arg("graph"),
            py::arg("reference_image"));

    /* *******************************************************************/

    const std::string transform_graph_to_index_space_docs = R"(
Transform graph to index space using an ITK image as reference for the metadata.
Uses Origin, Spacing and DirectionMatrix.

Assumes that the graph is currently in physical space.

Parameters:
----------
graph: GraphType
reference_image: Image (binary or float)
)";
    /* ************** BinaryImage ****************************/
    m.def(
            "transform_graph_to_index_space",
            [](const SG::GraphType &sg,
               SG::IUC3P input_image) -> SG::GraphType {
                SG::GraphType output_graph = sg;
                SG::transform_graph_to_index_space(output_graph,
                                                   input_image.GetPointer());
                return output_graph;
            },
            transform_graph_to_index_space_docs.c_str(), py::arg("graph"),
            py::arg("reference_image"));
    /* ************** FloatImage ****************************/
    m.def(
            "transform_graph_to_index_space",
            [](const SG::GraphType &sg, SG::IF3P input_image) -> SG::GraphType {
                SG::GraphType output_graph = sg;
                SG::transform_graph_to_index_space(output_graph,
                                                   input_image.GetPointer());
                return output_graph;
            },
            transform_graph_to_index_space_docs.c_str(), py::arg("graph"),
            py::arg("reference_image"));
}
