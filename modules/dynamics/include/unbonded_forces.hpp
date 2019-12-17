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

#ifndef SG_UNBONDED_FORCES_HPP
#define SG_UNBONDED_FORCES_HPP

#include "array_utilities.hpp"
#include <math.h>

namespace SG {

/**
 * Simple linear viscous force, proportional to velocity.
 * \f[ \vec{F} = - \mu \cdot \vec{v} \f]
 *
 * @param damping_parameter has to be positive
 * @param velocity
 *
 * @return force
 */
inline ArrayUtilities::Array3D
force_linear_drag(double damping_parameter,
                  const ArrayUtilities::Array3D &velocity) {
    return ArrayUtilities::product_scalar(velocity, -damping_parameter);
}
/**
 * Use the Stokes relationship for spherical particles of radius
 * @sphere_radius moving in a viscous fluid
 *
 * @param sphere_radius
 * @param fluid_viscosity
 * @param velocity
 *
 * @return
 * @sa force_linear_drag
 */
inline ArrayUtilities::Array3D
force_linear_drag(double sphere_radius,
                  double fluid_viscosity,
                  const ArrayUtilities::Array3D &velocity) {
    return force_linear_drag(6.0 * M_PI * sphere_radius * fluid_viscosity,
                             velocity);
}

// TODO: Add Newtonian fluid: shear_stress = shear_viscosity * du/dy
// where du/dy is the derivative of the velocity component that is parallel
// to the direction of shear, relative to displacement in the perpendicular
// direction. https://en.wikipedia.org/wiki/Newtonian_fluid

} // end namespace SG
#endif
