/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
