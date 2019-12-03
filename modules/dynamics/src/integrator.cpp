/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "integrator.hpp"

namespace SG {

void Integrator::compute_net_forces(System & sys) const {
    const auto nparts = sys.all.particles.size();
    for (size_t part_index = 0; part_index < nparts; ++part_index) {
        auto & net_force = sys.all.particles[part_index].dynamics.net_force;
        net_force = ArrayUtilities::Array3D(); // zero
        for (auto &force_type : force_types) {
            net_force = ArrayUtilities::plus(
                    net_force, force_type->forces[part_index]);
        };
    }
}

void IntegratorTwoStep::update(unsigned int time_step) {
    // sum all forces affecting every particles and
    // perform integration to get positions from forces.
    if(!integrator_method) {
        throw std::runtime_error("Provide an integrator method to Integrator");
    }
    this->integrator_method->integrateStepOne();
    // compute all types of forces
    for(auto & f : this->force_types) {
        f->compute();
        std::cout << ArrayUtilities::to_string(f->forces[2]) << std::endl;
    }
    // sum net forces
    this->compute_net_forces(m_sys);
    this->integrator_method->integrateStepTwo();

};

void VerletVelocitiesIntegratorMethod::integrate() {
    std::cout << "Do Nothing, update() in Integrator has to compute net_forces" << std::endl;
}
void VerletVelocitiesIntegratorMethod::integrateStepOne() {
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

void VerletVelocitiesIntegratorMethod::integrateStepTwo() {
    // first update acceleration from current forces
    // a(t+deltaT) = force/mass
    // v(t+deltaT) = v(t+deltaT/2) + 1/2 * a(t+deltaT)*deltaT
    const auto nparts = m_sys.all.particles.size();
    for (unsigned int index = 0; index < nparts; ++index) {
        // auto & position = m_sys.all.particles[index].pos;
        auto &velocity = m_sys.all.particles[index].dynamics.vel;
        auto &acceleration = m_sys.all.particles[index].dynamics.acc;
        auto &force = m_sys.all.particles[index].dynamics.net_force;
        const auto &mass = m_sys.all.particles[index].material.mass;
        acceleration = ArrayUtilities::product_scalar(force, 1.0 / mass);
        velocity = ArrayUtilities::plus(
                velocity,
                ArrayUtilities::product_scalar(acceleration, deltaT * 0.5));
    }
}
}; // namespace SG
