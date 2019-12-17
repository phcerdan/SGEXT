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

#include "system.hpp"
#include "sgdynamics_common_py.hpp"

#include <pybind11/pybind11.h>

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
