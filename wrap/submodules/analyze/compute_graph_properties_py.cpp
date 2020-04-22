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
#include <pybind11/stl.h>

#include "compute_graph_properties.hpp"

namespace py = pybind11;
using namespace SG;

void init_compute_graph_properties(py::module &m) {
    m.def("compute_cosines", &compute_cosines);
    m.def("compute_degrees", &compute_degrees);
    m.def("compute_ete_distances", &compute_ete_distances,
            py::arg("graph"),
            py::arg("min_edge_points") = 0,
            py::arg("ignore_end_nodes") = false
            );
    m.def("compute_contour_lengths", &compute_contour_lengths,
            py::arg("graph"),
            py::arg("min_edge_points") = 0,
            py::arg("ignore_end_nodes") = false
            );
    m.def("compute_angles", &compute_angles,
            py::arg("graph"),
            py::arg("min_edge_points") = 0,
            py::arg("ignore_parallel_edges") = false,
            py::arg("ignore_end_nodes") = false
            );
}
