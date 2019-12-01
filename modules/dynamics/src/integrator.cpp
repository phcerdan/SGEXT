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

#include "integrator.hpp"

namespace SG {
decltype(ForceCompute::forces) Integrator::sum_all_forces() const {
    const auto nparts = m_sys.all.particles.size();
    decltype(ForceCompute::forces) force_sum(nparts);
    for (size_t part_index = 0; part_index < nparts; ++part_index) {
        for (auto &force_type : force_types) {
            force_sum[part_index] = ArrayUtilities::plus(
                    force_sum[part_index], force_type->forces[part_index]);
        };
    }
    return force_sum;
};

void Integrator::update(unsigned int time_step) {
    // sum all forces affecting every particles and
    // perform integration to get positions from forces.
    if(!integrator_method) {
        throw std::runtime_error("Provide an integrator method to Integrator");
    }
    this->integrator_method->integrate(sum_all_forces());
};

void VerletVelocitiesMethod::integrateStepOne() {
    // perform the first half step of velocity verlet
    // r(t+deltaT) = r(t) + v(t)*deltaT + (1/2)a(t)*deltaT^2
    // v(t+deltaT/2) = v(t) + (1/2)a*deltaT
    const auto nparts = m_sys.all.particles.size();
    for (unsigned int index = 0; index < nparts; ++index) {
        auto &position = m_sys.all.particles[index].pos;
        auto &velocity = m_sys.all.particles[index].dynamics.vel;
        const auto &acceleration = m_sys.all.particles[index].dynamics.acc;
        const ArrayUtilities::Array3D dr = ArrayUtilities::plus(
                ArrayUtilities::product_scalar(velocity, deltaT),
                ArrayUtilities::product_scalar(acceleration,
                                               deltaT * deltaT * 0.5));
        // TODO limit the movement of the particles?
        position = ArrayUtilities::plus(position, dr);
        velocity = ArrayUtilities::plus(
                velocity,
                ArrayUtilities::product_scalar(acceleration, deltaT * 0.5));
    }
}

void VerletVelocitiesMethod::integrateStepTwo(const decltype(ForceCompute::forces) &forces) {
    // first update acceleration from current forces
    // a(t+deltaT) = force/mass
    // v(t+deltaT) = v(t+deltaT/2) + 1/2 * a(t+deltaT)*deltaT
    const auto nparts = m_sys.all.particles.size();
    for (unsigned int index = 0; index < nparts; ++index) {
        // auto & position = m_sys.all.particles[index].pos;
        auto &velocity = m_sys.all.particles[index].dynamics.vel;
        auto &acceleration = m_sys.all.particles[index].dynamics.acc;
        auto &force = forces[index];
        const auto &mass = m_sys.all.particles[index].material.mass;
        acceleration = ArrayUtilities::product_scalar(force, 1.0 / mass);
        velocity = ArrayUtilities::plus(
                velocity,
                ArrayUtilities::product_scalar(acceleration, deltaT * 0.5));
    }
}

void VerletVelocitiesMethod::integrate(const decltype(ForceCompute::forces) &forces) {
    integrateStepOne();
    integrateStepTwo(forces);
}
}; // namespace SG
