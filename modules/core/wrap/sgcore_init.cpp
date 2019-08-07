/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <pybind11/pybind11.h>

namespace py = pybind11;
void init_array3d(py::module &);
void init_spatial_node(py::module &);
void init_spatial_edge(py::module &);
void init_spatial_graph(py::module &);

PYBIND11_MODULE(sgcore, m) {
    m.doc() = "SGCore "; // optional module docstring
    init_array3d(m);
    init_spatial_node(m);
    init_spatial_edge(m);
    init_spatial_graph(m);
}
