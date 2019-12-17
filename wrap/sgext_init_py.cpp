/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <pybind11/pybind11.h>
namespace py = pybind11;

void init_sgcore(py::module &);
void init_sggenerate(py::module &);
void init_sgdynamics(py::module &);

PYBIND11_MODULE(_sgext, m) {
    m.doc() = "SGEXT, Spatial Graph Extraction, Analysis and Generation";
    init_sgcore(m);
    init_sggenerate(m);
    init_sgdynamics(m);
}
