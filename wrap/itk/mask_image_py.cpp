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
