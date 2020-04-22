/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
