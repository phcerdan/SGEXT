/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

#include "graph_data.hpp"

namespace py = pybind11;
using namespace SG;

void init_graph_data(py::module &mparent) {
    auto m = mparent.def_submodule("io");
    m.def(
            "read_graph_data",
            [](const std::string &filename) {
                return read_graph_data(filename);
            },
            R"(
Read data from a data filename of format:

# header
value value value ...
# other header
value value value ...
...

Returns vector[pair [header, vector<double>]]

Parameters:
----------
filename: String
input filename

)",
            py::arg("filename"));
}
