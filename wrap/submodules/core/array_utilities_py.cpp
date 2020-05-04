/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

#include "array_utilities.hpp"

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
