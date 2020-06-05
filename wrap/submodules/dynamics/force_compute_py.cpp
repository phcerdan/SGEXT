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
#include "force_compute.hpp"
#include "pyforce_compute.hpp" // For trampolin pure virtual PyForceCompute


namespace py = pybind11;
using namespace SG;

void init_particle_force(py::module &m);
void init_ParticleForceCompute(py::module &m);
void init_ParticleRandomForceCompute(py::module &m);
void init_bond_force(py::module &m);
void init_PairBondForce(py::module &m);
void init_FixedPairBondForce(py::module &m);
// Wrap to avoid including pybind11/funtional.h in this translation unit
void wrap_force_function_with_functional(py::class_<ParticleForceCompute, ForceCompute,
        std::shared_ptr<ParticleForceCompute>> &c);
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
    init_ParticleForceCompute(m);
    init_ParticleRandomForceCompute(m);
    init_bond_force(m);
    init_PairBondForce(m);
    init_FixedPairBondForce(m);
}

void init_particle_force(py::module &m) {
    py::class_<ParticleForce>(m, "particle_force")
        .def(py::init<>())
        .def(py::init<const size_t &, const ArrayUtilities::Array3D &>())
        .def_readwrite("particle_id", &ParticleForce::particle_id)
        .def_readwrite("force", &ParticleForce::force);
}

void init_ParticleForceCompute(py::module &m) {
    auto force_compute_class = py::class_<ParticleForceCompute, ForceCompute,
        std::shared_ptr<ParticleForceCompute>>(m, "force_compute_particle")
        .def(py::init<const System *>())
        .def(py::init<const System *, ParticleForceCompute::force_function_t>())
        ;
    wrap_force_function_with_functional(force_compute_class);
}

void init_ParticleRandomForceCompute(py::module &m) {
    auto force_compute_class =
            py::class_<ParticleRandomForceCompute, ParticleForceCompute,
                       std::shared_ptr<ParticleRandomForceCompute>>(
                    m, "force_compute_particle_random");
    force_compute_class.def(py::init<const System *>());
    force_compute_class.def(
            py::init<const System *,
                     ParticleRandomForceCompute::force_function_t>());
    force_compute_class.def(
            py::init<const System *, const double &, const double &,
                     const double &, const size_t &>(),
            py::arg("sys"), py::arg("kT"), py::arg("gamma"), py::arg("deltaT"),
            py::arg("dimension") = 3);
    force_compute_class.def_readonly("modulo", &ParticleRandomForceCompute::_modulo);
    force_compute_class.def_readonly("kT", &ParticleRandomForceCompute::kT);
    force_compute_class.def_readonly("gamma", &ParticleRandomForceCompute::gamma);
    force_compute_class.def_readonly("dimension", &ParticleRandomForceCompute::dimension);
    force_compute_class.def_readonly("deltaT", &ParticleRandomForceCompute::deltaT);
    ;
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
        .def(py::init<const System *, const BondProperties::tags_t &>())
        .def(py::init<const System *, PairBondForce::force_function_t>())
        .def(py::init<const System *, PairBondForce::force_function_t, const BondProperties::tags_t &>())
        .def_readwrite("bond_forces", &PairBondForce::bond_forces)
        .def("only_apply_to_bonds_with_tags", &PairBondForce::only_apply_to_bonds_with_tags,
                "Only apply the force if the bonds share any tag with the input tags")
        ;
    wrap_force_function_with_functional(force_compute_class);
}

void init_FixedPairBondForce(py::module &m) {
    auto force_compute_class = py::class_<FixedPairBondForce,
         PairBondForce,
         std::shared_ptr<FixedPairBondForce>>(m, "force_compute_fixed_pair_bond")
        .def("get_type", &FixedPairBondForce::get_type)
        .def("compute", &FixedPairBondForce::compute)
        .def("compute_once", &FixedPairBondForce::compute_once)
        .def("negate_forces", &FixedPairBondForce::negate_forces);
    // wrap_force_function_with_functional(force_compute_class);
}
