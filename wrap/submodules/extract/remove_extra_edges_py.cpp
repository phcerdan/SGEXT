/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
// #include "sgextract_common_py.hpp"
#include "remove_extra_edges.hpp"

namespace py = pybind11;
using namespace SG;

void init_remove_extra_edges(py::module &m) {
    m.def("remove_extra_edges", &remove_extra_edges);
}
