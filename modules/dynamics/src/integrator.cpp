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

void Integrator::compute_net_forces(System *sys) const {
    const auto nparts = sys->all.particles.size();
    for (size_t part_index = 0; part_index < nparts; ++part_index) {
        auto &net_force = sys->all.particles[part_index].dynamics.net_force;
        std::fill(net_force.begin(), net_force.end(), 0.0);
        for (const auto &force_type : force_types) {
            net_force = ArrayUtilities::plus(
                    net_force, force_type->particle_forces[part_index].force);
        };
    }
}

void IntegratorTwoStep::update(unsigned int /* time_step */) {
    // sum all forces affecting every particles and
    // perform integration to get positions from forces.
    if (!integrator_method) {
        throw std::runtime_error("Provide an integrator method to Integrator");
    }
    this->integrator_method->integrateStepOne();
    // compute all types of forces
    for (auto &force_type : this->force_types) {
        force_type->compute();
        { // TODO remove debug
            std::cout << "force_type: " << force_type->get_type() << std::endl;
            for (auto &particle_force : force_type->particle_forces) {
                std::cout << "Particle: " << particle_force.particle_id
                          << ", force: "
                          << ArrayUtilities::to_string(particle_force.force)
                          << std::endl;
            }
        }
    }
    // sum net forces
    this->compute_net_forces(m_sys);
    this->integrator_method->integrateStepTwo();
    { // TODO remove debug
        for (auto &particle : m_sys->all.particles) {
            std::cout << "id: " << particle.id << "; net_force: "
                      << ArrayUtilities::to_string(particle.dynamics.net_force)
                      << std::endl;
        }
    }
}

void VerletVelocitiesIntegratorMethod::integrateStepOne() {
    // perform the first half step of velocity verlet
    // r(t+deltaT) = r(t) + v(t)*deltaT + (1/2)a(t)*deltaT^2
    // v(t+deltaT/2) = v(t) + (1/2)a*deltaT
    const auto nparts = m_sys->all.particles.size();
    for (unsigned int index = 0; index < nparts; ++index) {
        auto &position = m_sys->all.particles[index].pos;
        auto &velocity = m_sys->all.particles[index].dynamics.vel;
        const auto &acceleration = m_sys->all.particles[index].dynamics.acc;
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

void VerletVelocitiesIntegratorMethod::integrateStepTwo() {
    // first update acceleration from current forces
    // a(t+deltaT) = force/mass
    // v(t+deltaT) = v(t+deltaT/2) + 1/2 * a(t+deltaT)*deltaT
    const auto nparts = m_sys->all.particles.size();
    for (unsigned int index = 0; index < nparts; ++index) {
        // auto & position = m_sys->all.particles[index].pos;
        auto &velocity = m_sys->all.particles[index].dynamics.vel;
        auto &acceleration = m_sys->all.particles[index].dynamics.acc;
        const auto &force = m_sys->all.particles[index].dynamics.net_force;
        const auto &mass = m_sys->all.particles[index].material.mass;
        acceleration = ArrayUtilities::product_scalar(force, 1.0 / mass);
        velocity = ArrayUtilities::plus(
                velocity,
                ArrayUtilities::product_scalar(acceleration, deltaT * 0.5));
    }
}
} // namespace SG
