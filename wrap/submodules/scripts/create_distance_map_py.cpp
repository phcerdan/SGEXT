/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

#include "create_distance_map_function.hpp"

namespace py = pybind11;
using namespace SG;
void init_create_distance_map(py::module &m) {
    m.def("create_distance_map", &create_distance_map_function,
            R"delimiter(
Create distance map using DGtal (high precision).
Returns dist_map image.

Parameters:
----------
input: Image
    input image generated from sgext functions.

use_itk: bool
    use itk for computing the distance map

verbose: bool
    extra information displayed during the algorithm.
            )delimiter",
            py::arg("input"),
            py::arg("use_itk") = false,
            py::arg("verbose") = false
         );
    m.def("create_distance_map_io", &create_distance_map_function_io,
            R"delimiter(
Create distance map using DGtal (high precision). Read/write from/to file.
Parameters:
----------
input_file: str
    input filename holding a binary image.

out_folder: str
    output folder to store the results.

foreground: str
    [white, black]
    Invert image if foreground voxels are black.

use_itk: bool
    use itk for computing the distance map

verbose: bool
    extra information displayed during the algorithm.
            )delimiter",
            py::arg("input_file"),
            py::arg("out_folder"),
            py::arg("foreground") = "white",
            py::arg("use_itk") = false,
            py::arg("verbose") = false
         );

}
