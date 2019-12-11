/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_FORCE_COMPUTE_HPP
#define SG_FORCE_COMPUTE_HPP

#include "system.hpp"

namespace SG {
struct ParticleForce {
    size_t particle_id;
    ArrayUtilities::Array3D force;
    ParticleForce() = default;
    ParticleForce(const size_t & input_particle_id, const ArrayUtilities::Array3D & input_force) :
      particle_id(input_particle_id), force(input_force) {}
    ParticleForce(const ParticleForce&) = default;
    ParticleForce(ParticleForce&&) = default;
    ParticleForce& operator=(const ParticleForce&) = default;
    ParticleForce& operator=(ParticleForce&&) = default;
    ~ParticleForce() = default;
};
/**
 * Force per particle
 */
struct ForceCompute {
    explicit ForceCompute(const System &sys) : m_sys(sys) {
        particle_forces.reserve(sys.all.particles.size());
        for (const auto &p : sys.all.particles) {
            particle_forces.emplace_back(p.id, ArrayUtilities::Array3D());
        }
    };
    virtual ~ForceCompute(){};
    // Compute and populate force;
    virtual void compute() = 0;
    std::vector<ParticleForce> particle_forces;

  protected:
    const System &m_sys;
};

/**
 * ForceCompute between a pair of Particles
 */
struct PairBondForce : public ForceCompute {
    using force_function_t = std::function<ArrayUtilities::Array3D(
            const Particle &, const Particle &, const Bond &)>;
    using ForceCompute::ForceCompute;
    PairBondForce(const System &sys, force_function_t force_function)
            : ForceCompute(sys), force_function(force_function) {}
    force_function_t force_function;
    void compute() override;

  protected:
    const ParticleNeighborsCollection &conexions = m_sys.conexions;
};

} // namespace SG
#endif
