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
    auto mtags = m.def_submodule("tags", "Submodule containing values for tags");
    mtags.def("tag_bond_int_to_string", &tag_bond_int_to_string,
            "Convert a tag to a pretty string");
    mtags.def("tag_bond_string_to_int", &tag_bond_string_to_int,
            "Convert a tag from string to an int (usable in bond_properties).");
    mtags.attr("chain") = pybind11::int_(tag_bond_chain);
    mtags.attr("free_chain") = pybind11::int_(tag_bond_free_chain);
    mtags.attr("contour_length_chain") = pybind11::int_(tag_bond_contour_length_chain);

    py::class_<BondProperties, std::shared_ptr<BondProperties>>(m, "bond_properties")
        .def(py::init())
        .def(py::init<const BondProperties::tag_t &>())
        .def(py::init<const BondProperties::tags_t &>())
        .def_readwrite("tags", &BondProperties::tags)
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
        .def(py::init<const BondProperties::tag_t &, const double &, const double &>())
        .def(py::init<const BondProperties::tags_t &, const double &, const double &>())
        .def(py::init<const double &, const double &>())
        .def_readwrite("persistence_length", &BondPropertiesPhysical::persistence_length)
        .def_readwrite("kT", &BondPropertiesPhysical::kT)
        .def("__str__", [](const BondPropertiesPhysical &properties) {
                std::stringstream os;
                print(properties, os);
                return os.str();
        });
}
