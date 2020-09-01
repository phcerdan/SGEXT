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
