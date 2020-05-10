/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

#include "fill_holes_function.hpp"

namespace py = pybind11;
using namespace SG;

void init_fill_holes(py::module &m) {
const std::string fill_holes_docs =
R"delimiter(
Parameters:
----------
input: BinaryImageType
    input binary image.

radius: int
    radius that define the neighborhood
    radius = 1 means a radius=1,1,1 in 3D, so the neighborhood volume
    will be: 3x3x3 (27 voxels).

    Defaults to 1.

majority: int
    Majority is the number of pixels in the neighborhood
    of an OFF pixel, to turn it into ON.
    By default majority = 1, this means that an off pixel will be turned on
    if in the neighborhood (set by radius) there are at least 50% + 1 pixels ON.
    If radius = 1,1, neighborhood size will be 3x3 = 9 pixels.
    If 5 pixels around an OFF pixel are ON, then it will be switched.
    If radius = 1,1,1 neighborhood size will be 3x3x3 = 27 voxels.
    If 14 pixels around an OFF pixel are ON, then it will be switched.

    Defaults to 3.

iterations: int
    Number of iterations for the voting algorithm.
    It defaults to infinity (1000).
    The output should be controlled by radius and majority parameters.

    Defaults to 1000.

verbose: bool
    extra information displayed during the algorithm.

)delimiter";
    m.def("fill_holes", &fill_holes_voting_iterative_function,
            fill_holes_docs.c_str(),
            py::arg("input"),
            py::arg("radius") = 1,
            py::arg("majority") = 3,
            py::arg("iterations") = 1000,
            py::arg("verbose") = false
         );
}
