/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
    constexpr double m_pi = 3.14159265358979323846;
    return force_linear_drag(6.0 * m_pi * sphere_radius * fluid_viscosity,
                             velocity);
}

// TODO: Add Newtonian fluid: shear_stress = shear_viscosity * du/dy
// where du/dy is the derivative of the velocity component that is parallel
// to the direction of shear, relative to displacement in the perpendicular
// direction. https://en.wikipedia.org/wiki/Newtonian_fluid

} // end namespace SG
#endif
