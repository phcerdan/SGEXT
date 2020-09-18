/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include "image_types.hpp"

#include "mask_image_function.hpp"
#include <string>

namespace py = pybind11;
using namespace SG;

const std::string mask_image_docs =
R"delimiter(
Masks input image (Binary or Float) with a binary image.

Parameters:
----------
input: BinaryImageType | FloatImageType
    input image.

mask: BinaryImageType
    binary image used to mask the input.
)delimiter";

void init_mask_image(py::module &m) {
    m.def("mask_image",
            &SG::mask_image_function<BinaryImageType, BinaryImageType>,
            mask_image_docs.c_str(),
            py::arg("input"),
            py::arg("mask")
         );
    m.def("mask_image",
            &SG::mask_image_function<FloatImageType, BinaryImageType>,
            mask_image_docs.c_str(),
            py::arg("input"),
            py::arg("mask")
         );
}
