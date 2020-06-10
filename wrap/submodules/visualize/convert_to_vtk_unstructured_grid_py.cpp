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
