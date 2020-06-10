/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

#include "boundary_conditions.hpp"

namespace py = pybind11;
using namespace ArrayUtilities;

void init_boundary_conditions(py::module &m) {
    py::enum_<boundary_condition>(m, "boundary_condition")
        .value("NONE", boundary_condition::NONE)
        .value("PERIODIC", boundary_condition::PERIODIC);
}
