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
