/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "sgitk_common_py.hpp"
#include "declare_itk_image_py.h"

namespace py = pybind11;

void init_itk_image(py::module &m) {
    declare_itk_image_ptr<SG::IUC3P>(m, "IUC3P");
    declare_itk_image_ptr<SG::IF3P>(m, "IF3P");
}
