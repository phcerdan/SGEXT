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
 * force_extension_wlc_petrosyan_normalized(relative_extension).
 * Where relative_extension = end_to_end_distance / contour_length
 * The direction of the force is from the first particle to the second (F_{ab})
 *
 * @param a first particle
 * @param b second particle
 * @param chain bond holding a contour length
 *
 * @return F_{a,b}
 */
ArrayUtilities::Array3D force_function_wlc_petrosyan_normalized(
        const SG::Particle &a, const SG::Particle &b, const SG::Bond &chain);
} // end namespace SG
#endif
