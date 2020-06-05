/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "force_compute.hpp"
#include "particle_collection.hpp"
#include "rng.hpp" // from core module

namespace SG {
void PairBondForce::compute() {
    if (!force_function) {
        throw std::runtime_error(
                "force_function is not set in PairBondForce");
    }

    reset_forces_to_zero();
    reset_bond_forces_to_zero();

    // Compute and store forces per bond
    for (auto &bond_force : bond_forces) {
        const auto [p_a, p_a_index] =
                m_sys->all.find_particle_and_index(bond_force.bond->id_a);
        const auto [p_b, p_b_index] =
                m_sys->all.find_particle_and_index(bond_force.bond->id_b);
        // Assign to bond_force
        // Bond force is equal to F_{a,b}
        bond_force.force = force_function(*p_a, *p_b, *bond_force.bond);
        // Translating a bond force using as a reference the force
        // from particle a to b, (i.e. F_{a,b}) to each particle.
        // We divide the bond force by half, and apply each half to each particle,
        // changing the sign.
        // (the particles are always at the two ends of the bond).
        const auto half_bond_force =  ArrayUtilities::product_scalar(bond_force.force, 0.5);
        // Assign to the per particle forces
        auto &force_on_a = particle_forces[p_a_index].force;
        auto &force_on_b = particle_forces[p_b_index].force;
        // F_{a,b}
        force_on_a = ArrayUtilities::plus(force_on_a, half_bond_force);
        // change sign of bond_force for the F_{b,a}
        force_on_b = ArrayUtilities::minus(force_on_b, half_bond_force);
    }
};

void PairBondForce::only_apply_to_bonds_with_tags(
        const System *sys, const BondProperties::tags_t &in_tags) {
    for (const auto &bond : sys->bonds.bonds) {
        const auto &bond_tags = bond->properties->tags;
        std::vector<BondProperties::tags_t::value_type> tags_intersection;
        std::set_intersection(in_tags.begin(), in_tags.end(), bond_tags.begin(),
                              bond_tags.end(),
                              std::back_inserter(tags_intersection));
        if (!tags_intersection.empty()) {
            bond_forces.emplace_back(bond.get(), ArrayUtilities::Array3D());
        }
    }
}

void FixedPairBondForce::compute() {
    // do nothing, the forces are fixed and set in compute_once()
    if (!forces_are_populated) {
        throw std::runtime_error(
                "FixedPairBondForce forces need to be populated before "
                "calling compute(). Use compute_once(), or manually set "
                "forces_are_populated to true.");
    }
}
void FixedPairBondForce::compute_once() {
    PairBondForce::compute();
    this->forces_are_populated = true;
}
void FixedPairBondForce::negate_forces() {
    for (auto &pf : particle_forces) {
        pf.force = ArrayUtilities::negate(pf.force);
    }
    for (auto &bf : bond_forces) {
        bf.force = ArrayUtilities::negate(bf.force);
    }
}

void ParticleForceCompute::compute() {
    if (!force_function) {
        throw std::runtime_error("force_function is not set in ParticleForceCompute");
    }
    reset_forces_to_zero();

    size_t current_particle_index = 0;
    for (auto &particle : m_sys->all.particles) {
        auto &current_particle_force =
                particle_forces[current_particle_index].force;
        assert(particle.id ==
                       particle_forces[current_particle_index].particle_id &&
               "particle ids are not synchronized in "
               "PairBondeForce:compute(), they should be sorted as well as "
               "all the particles.");
        current_particle_force = ArrayUtilities::plus(current_particle_force,
                                                      force_function(particle));
        current_particle_index++;
    }
}

ParticleRandomForceCompute::ParticleRandomForceCompute(
        const System *sys,
        const double &kT,
        const double &gamma /* drag */,
        const double &deltaT,
        const size_t &dimension)
        : ParticleForceCompute(sys), kT(kT), gamma(gamma), deltaT(deltaT), dimension(dimension), _modulo(sqrt(2 * dimension * kT * gamma / deltaT)) {
    const auto &modulo = _modulo;
    this->force_function =
            [&modulo](const Particle &particle) -> ArrayUtilities::Array3D {
                return RNG::random_orientation(modulo);
    };
}

}; // namespace SG
