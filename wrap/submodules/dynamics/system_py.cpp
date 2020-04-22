/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

#include "system.hpp"
#include "sgdynamics_common_py.hpp"


namespace py = pybind11;
using namespace SG;

void init_system(py::module &m) {
    py::class_<System>(m, "system")
        .def(py::init())
        .def_readwrite("all", &System::all)
        .def_readwrite("bonds", &System::bonds)
        .def("__str__", [](const System &sys) {
                std::stringstream os;
                os << "particles: " << std::endl;
                print(sys.all, os);
                os << "bonds: " << std::endl;
                print(sys.bonds, os);
                return os.str();
        });
}
