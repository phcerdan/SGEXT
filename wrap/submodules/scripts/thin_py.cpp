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
