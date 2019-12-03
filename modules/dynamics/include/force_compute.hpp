/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_FORCE_COMPUTE_HPP
#define SG_FORCE_COMPUTE_HPP

#include "dynamics_common_data.hpp"

namespace SG {
/**
 * Force per particle
 */
struct ForceCompute {
    explicit ForceCompute(const System &sys) : m_sys(sys) {
        forces.resize(sys.all.particles.size());
    };
    virtual ~ForceCompute(){};
    // Compute and populate force;
    virtual void compute() = 0;
    std::vector<ArrayUtilities::Array3D> forces;

  protected:
    const System &m_sys;
};

/**
 * ForceCompute between a pair of Particles
 * Requires a 
 */
struct PairBondForce : public ForceCompute {
    using force_function_t = std::function<ArrayUtilities::Array3D(
            const Particle &, const Particle &)>;
    using ForceCompute::ForceCompute;
    PairBondForce(const System & sys, force_function_t force_function):
      ForceCompute(sys), force_function(force_function)
    { }
    force_function_t force_function;
    void compute() override;

  protected:
    const ParticleNeighborsCollection &bonds = m_sys.bonds;
};

} // namespace SG
#endif
