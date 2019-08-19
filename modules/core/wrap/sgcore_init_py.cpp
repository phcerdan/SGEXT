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

#include <pybind11/pybind11.h>

namespace py = pybind11;
void init_array3d(py::module &);
void init_spatial_node(py::module &);
void init_spatial_edge(py::module &);
void init_spatial_graph(py::module &);
void init_edge_points_utilities(py::module &);
void init_spatial_graph_io(py::module &);

PYBIND11_MODULE(sgcore, m) {
    m.doc() = "SGCore "; // optional module docstring
    init_array3d(m);
    init_edge_points_utilities(m);
    init_spatial_node(m);
    init_spatial_edge(m);
    init_spatial_graph(m);
    init_spatial_graph_io(m);
}
