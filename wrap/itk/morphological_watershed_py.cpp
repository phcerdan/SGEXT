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

#include "morphological_watershed.hpp"

namespace py = pybind11;
using namespace SG;

void init_morphological_watershed(py::module &m) {
    m.def("morphological_watershed", &morphological_watershed,
            R"(Performs a morphological watershed from markers to "refill" the original
binary image from a marker image that can have different labels,
marker_image is usually set to the output image of sgext.scripts.voxelize_graph.

Parameters:
----------

original_binary_image: BinaryImageType
  input original image that will set the boundaries to refill from markers

label_marker_image: BinaryImageType
  image with markers from for example sgext.scripts.voxelize_graph

Returns: BinaryImageType
  original_binary_image but with labels propagated from the marker image.)",
            py::arg("original_binary_image"),
            py::arg("label_marker_image")
            );
}
