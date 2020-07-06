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
