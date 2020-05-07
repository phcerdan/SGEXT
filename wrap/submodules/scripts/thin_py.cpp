/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

#include "thin_function.hpp"

namespace py = pybind11;
using namespace SG;
void init_thin(py::module &m) {
    m.def("thin", &thin_function,
            R"delimiter(
Get a skeletonized or thinned image from a binary image.

Parameters:
----------
input: BinaryImageType
    input binary image.

skel_type: str
    Voxels to keep in the skeletonization process.

    [end, ulti, isthmus]
    - end: keep end voxels.
    - ulti: don't keep extra voxels, ultimate skeleton.
    - isthmus: keep voxels that are isthmuses.

select_type: str
    [first, random, dmax]
    - first: the first voxel in the set (no criteria)
    - random: random voxel in the set.
    - dmax: choose voxel with greatest distance map value.
    Strategy to choose voxels in the asymmetric process.

persistence: int
    if >0, performs a persistence algorithm that prunes
    branches that are not persistant (less important).

input_distance_map_image: FloatImageType
    distance map required for select_type dmax option.
    This option provides a centered skeleton.
    Use sgext.scripts.create_distance_map function
    to generate it. Defaults to nullptr

profile: bool
    time the algorithm

verbose: bool
    extra information displayed during the algorithm.

visualize: bool
    visualize results when finished.
            )delimiter",
            py::arg("input"),
            py::arg("skel_type"),
            py::arg("select_type"),
            py::arg("persistence") = 0,
            py::arg("input_distance_map_image") = nullptr,
            py::arg("profile") = false,
            py::arg("verbose") = false,
            py::arg("visualize") = false
         );

    m.def("thin_io", &thin_function_io,
            R"delimiter(
Get a skeletonized or thinned image from a binary image.

Parameters:
----------
input_file: str
    input filename holding a binary image.

skel_type: str
    Voxels to keep in the skeletonization process.

    [end, ulti, isthmus]
    - end: keep end voxels.
    - ulti: don't keep extra voxels, ultimate skeleton.
    - isthmus: keep voxels that are isthmuses.

select_type: str
    [first, random, dmax]
    - first: the first voxel in the set (no criteria)
    - random: random voxel in the set.
    - dmax: choose voxel with greatest distance map value.
    Strategy to choose voxels in the asymmetric process.

out_folder: str
    output folder to store the results.

persistence: int
    if >0, performs a persistence algorithm that prunes
    branches that are not persistant (less important).

input_distance_map_file: str
    file holding a distance map. Required for select_type dmax option.
    This option provides a centered skeleton.
    Use sgext.scripts.create_distance_map function
    to generate it

foreground: str
    [white, black]
    Invert image if foreground voxels are black.

out_discrete_points_folder: str
    output skeleton points in a simple file

profile: bool
    time the algorithm

verbose: bool
    extra information displayed during the algorithm.

visualize: bool
    visualize results when finished.

            )delimiter",
            py::arg("input_file"),
            py::arg("skel_type"),
            py::arg("select_type"),
            py::arg("out_folder"),
            py::arg("persistence") = 0,
            py::arg("input_distance_map_file") = "",
            py::arg("foreground") = "white",
            py::arg("out_discrete_points_folder") = "",
            py::arg("profile") = false,
            py::arg("verbose") = false,
            py::arg("visualize") = false
         );
}
