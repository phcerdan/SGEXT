/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"

#include "bounding_box.hpp"

namespace py = pybind11;
using namespace SG;

void init_bounding_box(py::module &m) {
    py::class_<BoundingBox, std::shared_ptr<BoundingBox>>(m, "bounding_box")
        .def(py::init<>())
        .def(py::init<const PointType&, const PointType&>())
        .def(py::init<const PointType&, size_t>())
        .def(py::init<const PointType&, const std::array<size_t, 3>&, const bool>())
        .def(py::init<double, double, double, double, double, double>())
        .def_readwrite("ini", &BoundingBox::ini)
        .def_readwrite("end", &BoundingBox::end)
        .def("set_bounds",
                py::overload_cast<double, double, double, double, double, double>(
                &BoundingBox::SetBounds))
        .def("get_size", &BoundingBox::GetSize)
        .def("get_radius", &BoundingBox::GetRadius)
        .def("get_center", &BoundingBox::GetCenter)
        .def("is_point_inside", &BoundingBox::is_point_inside)
        .def("are_bounds_inside", [](const BoundingBox & this_box,
                    const BoundingBox & external_box) {
                return this_box.are_bounds_inside(external_box);
                })
        .def("__str__", [](const BoundingBox & box) {
                std::stringstream os;
                box.Print("", os);
                return os.str();
                })
        .def_static("build_enclosing_box",
                py::overload_cast<const std::vector<BoundingBox> &>(
                &BoundingBox::BuildEnclosingBox))
        ;
}
