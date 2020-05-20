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

#include "force_compute.hpp"
#include "particle_collection.hpp"

namespace SG {
void PairBondForce::compute() {
    if (!force_function) {
        throw std::runtime_error("force_function is not set in PairBondForce");
    }

    reset_forces_to_zero();

    size_t current_particle_index = 0;
    for (auto &particle : m_sys->all.particles) {
        auto bonds = m_sys->bonds.find_all_bonds_with_id(particle.id);
        for (const auto &bond : bonds) {
            const auto connected_particle_id =
                    (particle.id == bond->id_a) ? bond->id_b : bond->id_a;
            const auto [connected_particle_it, connected_particle_index] =
                    m_sys->all.find_particle_and_index(connected_particle_id);

            auto &current_particle_force =
                    particle_forces[current_particle_index].force;
            assert(particle.id == particle_forces[current_particle_index]
                                          .particle_id &&
                   "particle ids are not synchronized in "
                   "PairBondeForce:compute(), they should be sorted as well as "
                   "all the particles.");
            current_particle_force = ArrayUtilities::plus(
                    current_particle_force,
                    force_function(particle, *connected_particle_it, *bond));
        }
        current_particle_index++;
    }
    // for (auto &p : conexions) {
    //     // Find the particle index
    //     const auto [particle_it, particle_index] =
    //             m_sys->all.find_particle_and_index(p.particle_id);
    //     if (particle_index == std::numeric_limits<size_t>::max()) {
    //         throw std::runtime_error("The particle in bonds does not
    //         exist.");
    //     }
    //     const auto &current_particle = *particle_it;
    //     // Apply force_function between current particle and its neighbors
    //     // if there is a proper bond between them
    //     for (auto &neighbor_id : p.neighbors) {
    //         const auto [neighbor_particle_it, neighbor_particle_index] =
    //                 m_sys->all.find_particle_and_index(neighbor_id);
    //         const auto &current_neighbor = *neighbor_particle_it;
    //         auto &current_particle_force = forces[particle_index];
    //         // TODO filter by bond type
    //         // Find bond
    //         BondChain chain = {1, 2, 2.0};
    //         current_particle_force = ArrayUtilities::plus(
    //                 current_particle_force,
    //                 force_function(current_particle, current_neighbor,
    //                 chain));
    //     }
    // }
}

void PairBondForceWithBond::compute() {
    if (!force_function) {
        throw std::runtime_error(
                "force_function is not set in PairBondForceWithBond");
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
        bond_force.force = force_function(*p_a, *p_b, *bond_force.bond);
        // Assign to the per particle forces
        auto &force_on_a =
            particle_forces[p_a_index].force;
        auto &force_on_b =
            particle_forces[p_b_index].force;
        // Bond force is equal to F_{a,b}
        force_on_a =
            ArrayUtilities::plus(force_on_a, bond_force.force);
        // change sign of bond_force for the F_{b,a}
        force_on_b =
            ArrayUtilities::minus(force_on_b, bond_force.force);
    }
};

void FixedPairBondForceWithBond::compute() {
    // do nothing, the forces are fixed and set in compute_once()
    if (!forces_are_populated) {
        throw std::runtime_error(
                "FixedPairBondForceWithBond forces need to be populated before "
                "calling compute(). Use compute_once(), or manually set "
                "forces_are_populated to true.");
    }
}
void FixedPairBondForceWithBond::compute_once() {
    PairBondForceWithBond::compute();
    this->forces_are_populated = true;
}
void FixedPairBondForceWithBond::negate_forces() {
      for(auto & pf : particle_forces) {
        pf.force = ArrayUtilities::negate(pf.force);
      }
      for(auto & bf : bond_forces) {
        bf.force = ArrayUtilities::negate(bf.force);
      }
}

void ParticleForceCompute::compute() {
    if (!force_function) {
        throw std::runtime_error("force_function is not set in PairBondForce");
    }
    reset_forces_to_zero();

    size_t current_particle_index = 0;
    for (auto &particle : m_sys->all.particles) {
        auto &current_particle_force =
            particle_forces[current_particle_index].force;
        assert(particle.id == particle_forces[current_particle_index]
                .particle_id &&
                "particle ids are not synchronized in "
                "PairBondeForce:compute(), they should be sorted as well as "
                "all the particles.");
        current_particle_force = ArrayUtilities::plus(
                current_particle_force,
                force_function(particle));
        current_particle_index++;
    }
}

}; // namespace SG
