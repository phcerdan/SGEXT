/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
namespace py = pybind11;

void init_create_vertex_to_radius_map(py::module &);
void init_tree_generation(py::module &);

void init_sgtree(py::module & mparent) {
    auto m = mparent.def_submodule("tree");
    m.doc() = "Tree sumodule, analysis of generations in the branching process "
              "of spatial graphs";
    init_create_vertex_to_radius_map(m);
    init_tree_generation(m);
}
