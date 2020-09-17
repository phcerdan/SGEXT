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

void init_create_vertex_to_radius_map(py::module &);
void init_tree_generation(py::module &);

void init_sgtree(py::module & mparent) {
    auto m = mparent.def_submodule("tree");
    m.doc() = "Tree sumodule, analysis of generations in the branching process "
              "of spatial graphs";
    init_create_vertex_to_radius_map(m);
    init_tree_generation(m);
}
