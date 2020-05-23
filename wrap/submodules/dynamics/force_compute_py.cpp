/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pybind11_common.h"
#include "sgdynamics_common_py.hpp"
#include "force_compute.hpp"
#include "pyforce_compute.hpp" // For trampolin pure virtual PyForceCompute


namespace py = pybind11;
using namespace SG;

void init_particle_force(py::module &m);
void init_particle_force_compute(py::module &m);
void init_bond_force(py::module &m);
void init_PairBondForceWithBond(py::module &m);
void init_FixedPairBondForceWithBond(py::module &m);
// Wrap to avoid including pybind11/funtional.h in this translation unit
void wrap_force_function_with_functional(py::class_<ParticleForceCompute, ForceCompute,
        std::shared_ptr<ParticleForceCompute>> &c);
void wrap_force_function_with_functional(py::class_<PairBondForceWithBond, ForceCompute,
        std::shared_ptr<PairBondForceWithBond>> &c);
void wrap_force_function_with_functional(py::class_<PairBondForce, ForceCompute,
        std::shared_ptr<PairBondForce>> &c);

void init_force_compute(py::module &m) {
    py::class_<ForceCompute, PyForceCompute,
        std::shared_ptr<ForceCompute>>(m, "force_compute")
        .def(py::init<const System*>())
        .def_readwrite("particle_forces", &ForceCompute::particle_forces)
        .def("compute", &ForceCompute::compute)
        .def("reset_forces_to_zero", &ForceCompute::reset_forces_to_zero)
        .def("get_type", &ForceCompute::get_type)
        ;
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
    auto force_compute_class = py::class_<ParticleForceCompute, ForceCompute,
        std::shared_ptr<ParticleForceCompute>>(m, "force_compute_particle")
        .def(py::init<const System *>())
        .def(py::init<const System *, ParticleForceCompute::force_function_t>())
        ;
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
    auto force_compute_class = py::class_<PairBondForce, ForceCompute,
        std::shared_ptr<PairBondForce>>(m, "force_compute_pair_bond")
        .def(py::init<const System *>())
        .def(py::init<const System *, PairBondForce::force_function_t>())
        ;
    wrap_force_function_with_functional(force_compute_class);
}

void init_PairBondForceWithBond(py::module &m) {
    auto force_compute_class = py::class_<PairBondForceWithBond, ForceCompute,
         std::shared_ptr<PairBondForceWithBond>>(m, "force_compute_pair_bond_with_bond")
        .def(py::init<const System *>())
        .def(py::init<const System *, PairBondForceWithBond::force_function_t>())
        .def_readwrite("bond_forces", &PairBondForceWithBond::bond_forces)
        ;
    wrap_force_function_with_functional(force_compute_class);
}

void init_FixedPairBondForceWithBond(py::module &m) {
    auto force_compute_class = py::class_<FixedPairBondForceWithBond,
         PairBondForceWithBond,
         std::shared_ptr<FixedPairBondForceWithBond>>(m, "force_compute_fixed_pair_bond_with_bond")
        .def("get_type", &FixedPairBondForceWithBond::get_type)
        .def("compute", &FixedPairBondForceWithBond::compute)
        .def("compute_once", &FixedPairBondForceWithBond::compute_once)
        .def("negate_forces", &FixedPairBondForceWithBond::negate_forces);
    // wrap_force_function_with_functional(force_compute_class);
}
