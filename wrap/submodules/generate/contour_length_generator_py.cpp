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

#include "contour_length_generator.hpp"
#include "contour_length_generator_functions.hpp"

namespace py = pybind11;
using namespace SG;

void init_contour_length_generator(py::module &m) {
    m.def("generate_contour_length",
           &generate_contour_length);
    py::class_<contour_length_generator>(m, "contour_length_generator")
            .def("generate_contour_length",
                 &contour_length_generator::generate_contour_length);
}
