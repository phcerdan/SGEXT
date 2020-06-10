/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

#include "convert_to_vtk_unstructured_grid.hpp"
#include "spatial_graph.hpp"

namespace py = pybind11;
using namespace SG;

void init_convert_to_vtk_unstructured_grid(py::module &m) {
    m.def(
            "write_graph_to_vtk_unstructured_grid",
            [](const GraphType &sg, const std::string &file_name,
               const bool &with_edge_points) -> void {
                auto ugrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
                if (with_edge_points) {
                    ugrid = convert_to_vtk_unstructured_grid_with_edge_points(
                            sg);
                } else {
                    ugrid = convert_to_vtk_unstructured_grid(sg);
                }
                write_vtk_unstructured_grid(ugrid, file_name);
            },
            py::arg("graph"), py::arg("filename"),
            py::arg("with_edge_points") = false);
}
