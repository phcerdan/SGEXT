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
void init_analyze_graph(py::module &);
void init_thin(py::module &);
void init_create_distance_map(py::module &);
void init_mask_image(py::module &);
void init_fill_holes(py::module &);
#ifdef SG_MODULE_VISUALIZE_ENABLED
void init_visualize_spatial_graph(py::module &);
#endif

void init_sgscripts(py::module & mparent) {
    auto m = mparent.def_submodule("scripts");
    m.doc() = "Scripts submodule "; // optional module docstring
    init_analyze_graph(m);
    init_thin(m);
    init_create_distance_map(m);
    init_mask_image(m);
    init_fill_holes(m);
#ifdef SG_MODULE_VISUALIZE_ENABLED
    init_visualize_spatial_graph(m);
#endif
}
