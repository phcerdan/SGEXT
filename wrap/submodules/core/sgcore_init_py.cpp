/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

namespace py = pybind11;
void init_array3d(py::module &);
void init_boundary_conditions(py::module &);
void init_spatial_node(py::module &);
void init_spatial_edge(py::module &);
void init_spatial_graph(py::module &);
void init_edge_points_utilities(py::module &);
void init_spatial_graph_io(py::module &);
void init_bounding_box(py::module &);
void init_filter_spatial_graph(py::module &);
void init_transform_to_physical_point_without_itk(py::module &);
void init_graph_data(py::module &);

void init_sgcore(py::module & mparent) {
    auto m = mparent.def_submodule("core");
    m.doc() = "Core submodule "; // optional module docstring
    init_array3d(m);
    init_boundary_conditions(m);
    init_edge_points_utilities(m);
    init_spatial_node(m);
    init_spatial_edge(m);
    init_spatial_graph(m);
    init_spatial_graph_io(m);
    init_bounding_box(m);
    init_filter_spatial_graph(m);
    init_transform_to_physical_point_without_itk(m);
    init_graph_data(m);
}
