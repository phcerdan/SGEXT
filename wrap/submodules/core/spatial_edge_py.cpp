/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sgcore_common_py.hpp"
#include "spatial_edge.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

namespace py = pybind11;
using namespace SG;

void init_spatial_edge(py::module &m) {
    py::class_<SpatialEdge>(m, "spatial_edge")
            .def(py::init())
            .def_readwrite("edge_points", &SpatialEdge::edge_points)
            .def("__repr__", [](const SpatialEdge &sn) {
                std::stringstream os;
                print_edge_points(sn.edge_points, os);
                return "<spatial_edge: edge_points: " + os.str() + ">";
            });
}
