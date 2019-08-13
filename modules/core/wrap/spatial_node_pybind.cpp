/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sgcore_common.hpp"
#include "spatial_node.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace SG;

void init_spatial_node(py::module &m) {
    py::class_<SpatialNode>(m, "spatial_node")
            .def(py::init())
            .def_readwrite("id", &SpatialNode::id)
            .def_readwrite("pos", &SpatialNode::pos)
            .def("__repr__", [](const SpatialNode &sn) {
                return "<spatial_node id: " + std::to_string(sn.id) +
                       "; pos: " + ArrayUtilities::to_string(sn.pos) + " >";
            });
}

