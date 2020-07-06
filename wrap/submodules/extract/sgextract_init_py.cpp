/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

namespace py = pybind11;
void init_split_loop(py::module &);
void init_merge_nodes(py::module &);
void init_remove_extra_edges(py::module &);
void init_reduce_spatial_graph(py::module &);
void init_detect_clusters(py::module &);
void init_collapse_clusters(py::module &);

void init_sgextract(py::module & mparent) {
    auto m = mparent.def_submodule("extract");
    m.doc() = "Extract submodule "; // optional module docstring
    init_split_loop(m);
    init_merge_nodes(m);
    init_remove_extra_edges(m);
    init_reduce_spatial_graph(m);
    init_detect_clusters(m);
    init_collapse_clusters(m);
}
