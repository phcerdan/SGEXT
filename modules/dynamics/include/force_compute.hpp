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
    using ForceCompute::ForceCompute;
    using force_function_t = std::function<ArrayUtilities::Array3D(
            const Particle &, const Particle &)>;
    force_function_t force_function;
    void compute() override;

  protected:
    const ParticleNeighborsCollection &bonds = m_sys.bonds;
};

} // namespace SG
#endif
