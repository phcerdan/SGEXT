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

#include "dynamics_common_data.hpp"
namespace SG {

/**
 * Force per particle
 */
struct ForceCompute {
    ForceCompute(const System &sys) : m_sys(sys) {
        forces.resize(sys.all.particles.size());
    };
    virtual ~ForceCompute();
    // Compute and populate force;
    virtual void compute() = 0;
    std::vector<ArrayUtilities::Array3D> forces;

  protected:
    const System &m_sys;
};

/**
 */
struct PairBondForce : public ForceCompute {
    using ForceCompute::ForceCompute;
    using force_function_t = std::function<ArrayUtilities::Array3D(
            const Particle &, const Particle &)>;
    force_function_t force_function;
    void compute() override {
        if (!force_function) {
            throw std::runtime_error(
                    "force_function is not set in PairBondForce");
        }
        for (auto &p : bonds.collection) {
            // Find the particle index
            const auto [particle_it, particle_index] =
                    m_sys.all.find_particle_and_index(p.particle_id);
            if (particle_index == std::numeric_limits<size_t>::max()) {
                throw std::runtime_error("The particle in bonds does not exist "
                                         "in all particles");
            }
            const auto &current_particle = *particle_it;
            // Apply force_function between current particle and its neighbors
            for (auto &neighbor_id : p.neighbors) {
                const auto [neighbor_particle_it, neighbor_particle_index] =
                        m_sys.all.find_particle_and_index(neighbor_id);
                const auto &current_neighbor = *neighbor_particle_it;
                auto &current_particle_force = forces[particle_index];
                current_particle_force = ArrayUtilities::plus(
                        current_particle_force,
                        force_function(current_particle, current_neighbor));
            }
        }
    };

  protected:
    const ParticleNeighborsCollection &bonds = m_sys.bonds;
};
// TODO: An integrator should:
// 1. Collect, and sum all the forces affecting every particle.
// 2. Integrate equation to get new positions from current state of the system
//    plus the forces.
//
class Integrator {
  public:
    Integrator(System &sys) : m_sys(sys) {};
    void update(unsigned int time_step);

    /**
     * 
     *
     * @tparam TForceCompute
     * @param new_force
     *
     * @return 
     */
    template <typename TForceCompute>
    ForceCompute& add_force(const TForceCompute & new_force) {
      force_types.emplace_back(std::make_unique(new_force));
      return *force_types.back();
    }
    /** Multiple forces can act over a particle.
     * Each ForceCompute has a force per particle.
     * Pointers to avoid slicing of Derived classes
     * shared_ptr because it's easier for usability than unique_ptr,
     * allowing to create an instance of ForceCompute
     */
    std::vector<std::unique_ptr<ForceCompute>> force_types;
  protected:
    System &m_sys;
};
} // namespace SG
#endif
