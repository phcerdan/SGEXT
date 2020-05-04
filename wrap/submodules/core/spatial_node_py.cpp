/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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

