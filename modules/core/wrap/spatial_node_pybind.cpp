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

#include "sgcore_common.hpp"
#include "spatial_node.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace SG;

void init_spatial_node(py::module &m) {
    py::class_<SpatialNode>(m, "spatial_node")
            .def(py::init())
            .def_readwrite("label", &SpatialNode::label)
            .def_readwrite("pos", &SpatialNode::pos)
            .def("__repr__", [](const SpatialNode &sn) {
                return "<sgcore.spatial_node label: " + sn.label +
                       "; pos: " + ArrayUtilities::to_string(sn.pos) + ">";
            });
}

