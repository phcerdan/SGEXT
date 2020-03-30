/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "thin_function.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace SG;
void init_thin(py::module &m) {
    m.def("thin", &thin_function,
            "Get a skeletonized or thinned image from a binary image.",
            py::arg("input"),
            py::arg("skel_type"),
            py::arg("select_type"),
            py::arg("out_folder"),
            py::arg("persistence") = 0,
            py::arg("input_distance_map_file") = "",
            py::arg("foreground") = "white",
            py::arg("out_discrete_points_folder") = "",
            py::arg("profile") = false,
            py::arg("verbose") = false,
            py::arg("visualize") = false,
            py::arg("thresholdMin") = 0,
            py::arg("thresholdMax") = 255
         );
}
