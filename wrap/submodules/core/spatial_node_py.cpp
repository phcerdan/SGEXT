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

#include "spatial_node.hpp"

namespace py = pybind11;
using namespace SG;

void init_spatial_node(py::module &m) {
    py::class_<SpatialNode>(m, "spatial_node")
            .def(py::init())
            .def_readwrite("id", &SpatialNode::id)
            .def_readwrite("pos", &SpatialNode::pos)
            // Without OPAQUE Array3D we cannot modify readwrite property
            // with a function taken a reference as argument, such as pos[1] = 2
            .def("set_pos", [](SpatialNode &sn,
                        size_t index,
                        const SpatialNode::PointType::value_type & in_value
                        ) {
                    sn.pos[index] = in_value;
                    },
                "sn.pos[0] = 3.0 does not modify the position of the spatial node. "
                "Use sn.set_pos(0, 3.0) instead if only one index needs to be modified. "
                "Note that changing the whole position works as expected:\n"
                "sn.pos = [3,4,5] or sn.pos = sn2.pos"
                )
            .def("__repr__", [](const SpatialNode &sn) {
                return "<spatial_node id: " + std::to_string(sn.id) +
                       "; pos: " + ArrayUtilities::to_string(sn.pos) + " >";
            });
}

