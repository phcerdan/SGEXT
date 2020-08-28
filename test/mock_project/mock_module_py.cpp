/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
namespace py = pybind11;

void init_sgmock(py::module &);
PYBIND11_MODULE(_mock, m) {
    m.doc() = "Mock external module for SGEXT";
    init_sgmock(m);
}

void init_sgmock(py::module &m) {
    m.def("say_mock", [](){std::cout <<"Mock!" << std::endl;});
}

