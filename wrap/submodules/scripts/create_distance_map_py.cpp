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

verbose: bool
    extra information displayed during the algorithm.
            )delimiter",
            py::arg("input"),
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

verbose: bool
    extra information displayed during the algorithm.
            )delimiter",
            py::arg("input_file"),
            py::arg("out_folder"),
            py::arg("foreground") = "white",
            py::arg("verbose") = false
         );

}
