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
               const SG::IUC3P &input_image) -> SG::GraphType {
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
            [](const SG::GraphType &sg,
               const SG::IF3P &input_image) -> SG::GraphType {
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
               const SG::IUC3P &input_image) -> SG::GraphType {
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
            [](const SG::GraphType &sg,
               const SG::IF3P &input_image) -> SG::GraphType {
                SG::GraphType output_graph = sg;
                SG::transform_graph_to_index_space(output_graph,
                                                   input_image.GetPointer());
                return output_graph;
            },
            transform_graph_to_index_space_docs.c_str(), py::arg("graph"),
            py::arg("reference_image"));
}
