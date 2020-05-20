/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include "sgdynamics_common_py.hpp"
#include "force_compute.hpp"


namespace py = pybind11;
using namespace SG;

void init_particle_force(py::module &m);
void init_particle_force_compute(py::module &m);
void init_bond_force(py::module &m);
void init_PairBondForceWithBond(py::module &m);
void init_FixedPairBondForceWithBond(py::module &m);
// Wrap to avoid including pybind11/funtional.h in this translation unit
void wrap_force_function_with_functional(py::class_<ParticleForceCompute> &c);
void wrap_force_function_with_functional(py::class_<PairBondForceWithBond> &c);
void wrap_force_function_with_functional(py::class_<PairBondForce> &c);

void init_force_compute(py::module &m) {
    init_particle_force(m);
    init_particle_force_compute(m);
    init_bond_force(m);
    init_PairBondForceWithBond(m);
    init_FixedPairBondForceWithBond(m);
}

void init_particle_force(py::module &m) {
    py::class_<ParticleForce>(m, "particle_force")
        .def(py::init<>())
        .def(py::init<const size_t &, const ArrayUtilities::Array3D &>())
        .def_readwrite("particle_id", &ParticleForce::particle_id)
        .def_readwrite("force", &ParticleForce::force);
}

void init_particle_force_compute(py::module &m) {
    auto force_compute_class = py::class_<ParticleForceCompute>(m, "particle_force_compute")
        .def(py::init<const System *>())
        .def(py::init<const System *, ParticleForceCompute::force_function_t>())
        .def("get_type", &ParticleForceCompute::get_type)
        .def("reset_forces_to_zero", &ParticleForceCompute::reset_forces_to_zero)
        .def_readwrite("particle_forces", &ParticleForceCompute::particle_forces)
        .def("compute", &ParticleForceCompute::compute);
    wrap_force_function_with_functional(force_compute_class);
}

void init_bond_force(py::module &m) {
    py::class_<BondForce>(m, "bond_force")
        .def(py::init<>())
        .def(py::init<Bond *, const ArrayUtilities::Array3D &>())
        .def_readwrite("bond", &BondForce::bond)
        .def_readwrite("force", &BondForce::force);
}

void init_PairBondForce(py::module &m) {
    auto force_compute_class = py::class_<PairBondForce>(m, "pair_bond_force_compute")
        .def(py::init<const System *>())
        .def(py::init<const System *, PairBondForce::force_function_t>())
        .def("get_type", &PairBondForce::get_type)
        .def("reset_forces_to_zero", &PairBondForce::reset_forces_to_zero)
        .def("compute", &PairBondForce::compute);
    wrap_force_function_with_functional(force_compute_class);
}

void init_PairBondForceWithBond(py::module &m) {
    auto force_compute_class = py::class_<PairBondForceWithBond>(m, "pair_bond_force_with_bond_compute")
        .def(py::init<const System *>())
        .def(py::init<const System *, PairBondForceWithBond::force_function_t>())
        .def("get_type", &PairBondForceWithBond::get_type)
        .def("reset_forces_to_zero", &PairBondForceWithBond::reset_forces_to_zero)
        .def("reset_bond_forces_to_zero", &PairBondForceWithBond::reset_bond_forces_to_zero)
        .def_readwrite("bond_forces", &PairBondForceWithBond::bond_forces)
        .def("compute", &PairBondForceWithBond::compute);
    wrap_force_function_with_functional(force_compute_class);
}

void init_FixedPairBondForceWithBond(py::module &m) {
    auto force_compute_class = py::class_<FixedPairBondForceWithBond,
         PairBondForceWithBond>(m, "fixed_pair_bond_force_with_bond_compute")
        .def("get_type", &FixedPairBondForceWithBond::get_type)
        .def("compute", &FixedPairBondForceWithBond::compute)
        .def("compute_once", &FixedPairBondForceWithBond::compute_once)
        .def("negate_forces", &FixedPairBondForceWithBond::negate_forces);
    // wrap_force_function_with_functional(force_compute_class);
}
