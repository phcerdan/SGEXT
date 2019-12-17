/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bond_collection.hpp"
#include "sgdynamics_common_py.hpp"

#include <pybind11/pybind11.h>
// stl.h for binding bond_vector_t (std::vector<std::shared_ptr<Bond>))
#include <pybind11/stl.h>

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
