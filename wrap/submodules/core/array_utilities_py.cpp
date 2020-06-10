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

#include "array_utilities.hpp"
#include "boundary_conditions.hpp"

namespace py = pybind11;
using namespace ArrayUtilities;

void init_array3d(py::module &m) {
    auto marray = m.def_submodule("array");
    marray.def("cross_product", &cross_product);
    marray.def("dot_product", &dot_product);
    marray.def("norm", &norm);
    marray.def("angle", &angle);
    marray.def("plus", &ArrayUtilities::plus);
    marray.def("plus_scalar", &plus_scalar);
    marray.def("minus", &ArrayUtilities::minus);
    marray.def("minus_scalar", &minus_scalar);
    marray.def("distance", &distance);
    marray.def("cos_director", &cos_director);
    marray.def("to_string", &ArrayUtilities::to_string);
    marray.def("negate", &ArrayUtilities::negate);
};
