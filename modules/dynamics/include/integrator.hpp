/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_INTEGRATOR_HPP
#define SG_INTEGRATOR_HPP

#include <memory>
#include <vector>

#include "dynamics_common_data.hpp"
#include "force_compute.hpp"
namespace SG {

// TODO Add a particle_selector (with lambda), here or on Integrator.
// Integrator has IntegratorMethods
struct IntegratorMethod {
    explicit IntegratorMethod(System &sys, double deltaT_input)
            : m_sys(sys), deltaT(deltaT_input){};
    double deltaT;
    virtual void integrate(const decltype(ForceCompute::forces) &forces) = 0;

  protected:
    System &m_sys;
};

struct VerletVelocitiesMethod : public IntegratorMethod {
    using IntegratorMethod::IntegratorMethod;

    /**
     * @param forces Corresponding to the final forces on each particle
     * (sum of many forces).
     * The method is decoupled with the type of force.
     * TODO: right now it assumes all particles of the system
     */
    void integrate(const decltype(ForceCompute::forces) &forces) override;

protected:
    /**
     * Positions are moved to timestep+1 and velocities to timestep+1/2
     */
    void integrateStepOne();
    /**
     * Velocities are finally computed to timestep + 1
     *
     */
    void integrateStepTwo(const decltype(ForceCompute::forces) &forces);
};

// TODO: An integrator should:
// 1. Collect, and sum all the forces affecting every particle.
// 2. Integrate equation to get new positions from current state of the system
//    plus the forces.
//
class Integrator {
  public:
    explicit Integrator(System &sys) : m_sys(sys){};
    /**
     * TODO: Integrator should be an interface, update should be pure virtual
     */
    virtual void update(unsigned int time_step);

    /**
     * add force to the integrator
     *
     * returns a raw pointer
     *
     * @tparam TForceCompute
     * @param new_force
     *
     * @return
     */
    template <typename TForceCompute>
    TForceCompute *add_force(std::shared_ptr<TForceCompute> new_force) {
        force_types.push_back(std::move(new_force));
        return dynamic_cast<TForceCompute *>(force_types.back().get());
    }
    /** Multiple forces can act over a particle.
     * Each ForceCompute has a force per particle.
     * Pointers to avoid slicing of Derived classes
     * shared_ptr because it's easier for usability than unique_ptr,
     * allowing to create an instance of ForceCompute
     */
    std::vector<std::shared_ptr<ForceCompute>> force_types;
    // TODO only one method? Add more methods when/if added particle selector
    std::shared_ptr<IntegratorMethod> integrator_method;

  protected:
    System &m_sys;
    /// Compute the sum of forces for each particle
    decltype(ForceCompute::forces) sum_all_forces() const;
};

} // namespace SG
#endif
