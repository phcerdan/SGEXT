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
#include "sgdynamics_common_py.hpp"
#include "bond.hpp"

namespace py = pybind11;
using namespace SG;

void init_bond(py::module &m) {
    py::class_<BondProperties, std::shared_ptr<BondProperties>>(m, "bond_properties")
        .def(py::init())
        .def(py::init<const size_t &>())
        .def_readwrite("tag", &BondProperties::tag)
        .def("__str__", [](const BondProperties &properties) {
                std::stringstream os;
                print(properties, os);
                return os.str();
        });
    py::class_<Bond, std::shared_ptr<Bond>>(m, "bond")
        .def(py::init())
        .def(py::init<const size_t &, const size_t&>())
        .def_readwrite("id_a", &Bond::id_a)
        .def_readwrite("id_b", &Bond::id_b)
        .def_readwrite("properties", &Bond::properties)
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

    py::class_<BondPropertiesPhysical, std::shared_ptr<BondPropertiesPhysical>,
        BondProperties>(m, "bond_properties_physical")
        .def(py::init())
        .def(py::init<const size_t &>())
        .def(py::init<const size_t &, const double &, const double &>())
        .def(py::init<const double &, const double &>())
        .def_readwrite("persistence_length", &BondPropertiesPhysical::persistence_length)
        .def_readwrite("kT", &BondPropertiesPhysical::kT)
        .def("__str__", [](const BondPropertiesPhysical &properties) {
                std::stringstream os;
                print(properties, os);
                return os.str();
        });
}
