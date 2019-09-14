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

#include "array_utilities.hpp"
#include "sgcore_common_py.hpp"
#include <pybind11/pybind11.h>
// #include <pybind11/stl.h>

namespace py = pybind11;
using namespace ArrayUtilities;

void init_array3d(py::module &m) {
    auto marray = m.def_submodule("array");
    py::class_<Array3D>(marray, "array3d")
            .def(py::init())
            .def(py::init<double, double, double>())
            .def("__repr__",
                 [](const Array3D &a) {
                     return "<sgcore.array.array3d: [" +
                            ArrayUtilities::to_string(a) + "] >";
                 })
            .def("__len__", &Array3D::size)
            .def(
                    "__eq__",
                    [](const Array3D &a, const Array3D &b) -> bool {
                        return a == b;
                    },
                    "Check if two arrays are equal")
            .def("__le__",
                 [](const Array3D &a, const Array3D &b) -> bool {
                     return a <= b;
                 })
            .def("__lt__",
                 [](const Array3D &a, const Array3D &b) -> bool {
                     return a < b;
                 })
            .def("__add__",
                 [](const Array3D &a, const Array3D &b) {
                     return ArrayUtilities::plus(a, b);
                 })
            .def("__sub__",
                 [](const Array3D &a, const Array3D &b) {
                     return ArrayUtilities::minus(a, b);
                 })
            .def(
                    "__getitem__",
                    [](Array3D &a,
                       Array3D::size_type i) -> Array3D::value_type & {
                        if (i >= a.size())
                            throw py::index_error();
                        return a[i];
                    },
                    py::return_value_policy::reference_internal // ref +
                                                                // keepalive
                    )
            .def(
                    "__iter__",
                    [](Array3D &a) {
                        return py::make_iterator<
                                py::return_value_policy::reference_internal,
                                Array3D::iterator, Array3D::iterator,
                                Array3D::value_type &>(a.begin(), a.end());
                    },
                    py::keep_alive<0, 1>() /* Essential: keep list alive while
                                              iterator exists */
                    )
            .def("__setitem__",
                 [](Array3D &a, Array3D::size_type i,
                    const Array3D::value_type &t) {
                     if (i >= a.size())
                         throw py::index_error();
                     a[i] = t;
                 })
            .def("fill", &Array3D::fill)
            .def_property(
                    "x", [](Array3D &a) { return a[0]; },
                    [](Array3D &a, const double &d) -> void { a[0] = d; })
            .def_property(
                    "y", [](Array3D &a) { return a[1]; },
                    [](Array3D &a, const double &d) -> void { a[1] = d; })
            .def_property(
                    "z", [](Array3D &a) { return a[2]; },
                    [](Array3D &a, const double &d) -> void { a[2] = d; });
    marray.def("cross_product", &cross_product);
    marray.def("dot_product", &dot_product);
    marray.def("norm", &norm);
    marray.def("angle", &angle);
    marray.def("plus", &ArrayUtilities::plus);
    marray.def("plus_scalar", &plus_scalar);
    marray.def("minus", &ArrayUtilities::minus);
    marray.def("minus_scalar", &minus_scalar);
    marray.def("distance", &distance);
    marray.def("cos_director", &cos_director);
    marray.def("to_string", &ArrayUtilities::to_string);
};
