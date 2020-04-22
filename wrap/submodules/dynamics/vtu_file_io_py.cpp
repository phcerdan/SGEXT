/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include "sgdynamics_common_py.hpp"
#include "write_vtu_file.hpp"

namespace py = pybind11;
using namespace SG;

void init_vtu_file_io(py::module &m) {
    m.def("write_vtu_file", &SG::write_vtu_file);
    m.def("read_vtu_file", &SG::read_vtu_file);
}
