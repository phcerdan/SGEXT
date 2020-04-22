/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include "sgdynamics_common_py.hpp"
#include "bond.hpp"

namespace py = pybind11;
using namespace SG;

void init_bond(py::module &m) {
    py::class_<Bond, std::shared_ptr<Bond>>(m, "bond")
        .def(py::init())
        .def(py::init<const size_t &, const size_t&>())
        .def_readwrite("id_a", &Bond::id_a)
        .def_readwrite("id_b", &Bond::id_b)
        .def("sort", [](Bond& bond){
                sort(bond);
                })
        .def("__str__", [](const Bond &bond) {
                std::stringstream os;
                print(bond, os);
                return os.str();
        })
        .def("__eq__", [](const Bond &lhs, const Bond &rhs) -> bool {
                return lhs == rhs;
        })
        .def("__lt__", [](const Bond &lhs, const Bond &rhs) -> bool {
                return lhs < rhs;
        })
        .def("__le__", [](const Bond &lhs, const Bond &rhs) -> bool {
                return lhs < rhs || lhs == rhs;
        })
        .def("__gt__", [](const Bond &lhs, const Bond &rhs) -> bool {
                return rhs < lhs;
        })
        .def("__ge__", [](const Bond &lhs, const Bond &rhs) -> bool {
                return rhs < lhs || lhs == rhs;
        });

    m.def("sort", py::overload_cast<Bond &>(&sort), "Sort the bond: id_a < id_b");

    py::class_<BondChain, std::shared_ptr<BondChain>, Bond>(m, "bond_chain")
        .def(py::init())
        .def(py::init<const size_t &, const size_t&>())
        .def(py::init<const size_t &, const size_t&, const double &>())
        .def_readwrite("length_contour", &BondChain::length_contour);
}
