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

#ifndef SG_INTEGRATOR_HPP
#define SG_INTEGRATOR_HPP

#include <memory>
#include <vector>

#include "system.hpp"
#include "force_compute.hpp"
namespace SG {

// TODO Add a particle_selector (with lambda), here or on Integrator.
// Integrator has IntegratorMethods
struct IntegratorMethod {
    explicit IntegratorMethod(System &sys, double deltaT_input)
            : m_sys(sys), deltaT(deltaT_input){};
    virtual ~IntegratorMethod(){};
    double deltaT;
    virtual void integrate() = 0;

  protected:
    System &m_sys;
};

struct TwoStepIntegratorMethod : public IntegratorMethod {
    using IntegratorMethod::IntegratorMethod;
    // virtual void integrate() = 0;
    /**
     * Positions are moved to timestep+1 and velocities to timestep+1/2
     */
    virtual void integrateStepOne() = 0;
    /**
     * Velocities are finally computed to timestep + 1
     *
     */
    virtual void integrateStepTwo() = 0;
};

struct VerletVelocitiesIntegratorMethod : public TwoStepIntegratorMethod {
    using TwoStepIntegratorMethod::TwoStepIntegratorMethod;

    /**
     * TODO: doing nothing, the work is done in the integrator,
     * calling the integrateSteps in order
     */
    void integrate() override {};

    /**
     * Positions are moved to timestep+1 and velocities to timestep+1/2
     */
    void integrateStepOne() override;
    /**
     * Velocities are finally computed to timestep + 1
     *
     */
    void integrateStepTwo() override;
};

// 1. Collect, and sum all the forces affecting every particle.
// 2. Integrate equation to get new positions from current state of the system
//    plus the forces.
class Integrator {
  public:
    explicit Integrator(System &sys) : m_sys(sys){};
    virtual ~Integrator(){};
    virtual void update(unsigned int time_step) = 0;
    /// Compute the sum of forces for each particle and store it in Particle
    virtual void compute_net_forces(System &sys) const;

    /**
     * add force to the integrator
     *
     * returns a shared_ptr of the force type
     *
     * @tparam TForceCompute
     * @param new_force
     *
     * @return
     */
    template <typename TForceCompute>
    auto add_force(std::shared_ptr<TForceCompute> new_force) {
        force_types.push_back(std::move(new_force));
        return std::dynamic_pointer_cast<TForceCompute>(force_types.back());
    }
    /** Multiple forces can act over a particle.
     * Each ForceCompute has a force per particle.
     * Pointers to avoid slicing of Derived classes
     * shared_ptr because it's easier for usability than unique_ptr,
     * allowing to create an instance of ForceCompute
     */
    std::vector<std::shared_ptr<ForceCompute>> force_types;

  protected:
    System &m_sys;
};

struct IntegratorTwoStep : public Integrator {
    using Integrator::Integrator;
    // TODO only one method? Add more methods when/if added particle selector
    std::shared_ptr<TwoStepIntegratorMethod> integrator_method;
    void update(unsigned int time_step) override;
};

} // namespace SG
#endif
