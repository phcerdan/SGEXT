/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_FORCE_FUNCTIONS_HPP
#define SG_FORCE_FUNCTIONS_HPP

#include <functional>

#include "array_utilities.hpp"
#include "bond.hpp"
#include "bonded_forces.hpp"
#include "particle.hpp"

namespace SG {
using force_function_particles_with_bond_t =
        std::function<ArrayUtilities::Array3D(
                const Particle &, const Particle &, const Bond &)>;


/**
 * force_function computing relative_extension from contour length.

 * Returns an array representing the vector of the resulting force of applying
 * force_extension_wlc_petrosyan(relative_extension, persistence_length, kT)
 * Where relative_extension = end_to_end_distance / contour_length
 * The direction of the force is from the first particle to the second (F_{ab})
 *
 * Note that this function needs the bond to have properties derived from
 * BondPropertiesPhysical, which contains persistence_length and kT.
 * Those physical parameters should be set before calling this force.
 *
 * @param a first particle
 * @param b second particle
 * @param chain bond holding a contour length
 *
 * @return F_{a,b}
 */
ArrayUtilities::Array3D force_function_wlc_petrosyan(
        const SG::Particle &a, const SG::Particle &b, const SG::Bond &chain);
} // end namespace SG
#endif
