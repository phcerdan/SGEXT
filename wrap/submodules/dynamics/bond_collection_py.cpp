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
// stl.h for binding bond_vector_t (std::vector<std::shared_ptr<Bond>))
#include <pybind11/stl.h>
#include "sgdynamics_common_py.hpp"

#include "bond_collection.hpp"

namespace py = pybind11;
using namespace SG;

void init_bond_collection(py::module &m) {
    // py::class_<std::vector<std::shared_ptr<Bond>>>(m, "bond_vector_t")
    // .def(py::init<>())
    // .def("clear", &std::vector<std::shared_ptr<Bond>>::clear)
    // .def("pop_back", &std::vector<std::shared_ptr<Bond>>::pop_back)
    // // .def("append", &std::vector<std::shared_ptr<Bond>>::push_back)
    // // .def("push_back", &std::vector<std::shared_ptr<Bond>>::push_back)
    // // .def("insert", &std::vector<std::shared_ptr<Bond>>::insert)
    // .def("__len__", [](const std::vector<std::shared_ptr<Bond>> &v) { return v.size(); })
    // .def("__iter__", [](std::vector<std::shared_ptr<Bond>> &v) {
    //    return py::make_iterator(v.begin(), v.end());
    // }, py::keep_alive<0, 1>()) /* Keep vector alive while iterator is used */
    // ;



    py::class_<BondCollection>(m, "bond_collection")
        .def(py::init())
        .def_readwrite("bonds", &BondCollection::bonds)
        .def_readwrite("sorted", &BondCollection::sorted)
        .def("__str__", [](const BondCollection &collection) {
                std::stringstream os;
                print(collection, os);
                return os.str();
        });
}
