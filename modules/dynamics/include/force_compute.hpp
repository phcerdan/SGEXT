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
    inline void reset_forces_to_zero() {
      for(auto & pf : particle_forces) {
        std::fill(pf.force.begin(), pf.force.end(), 0);
      }
    }
    inline virtual std::string get_type() {return  "ForceCompute";};

  protected:
    const System &m_sys;
};

struct ParticleForceCompute : public ForceCompute {
    inline virtual std::string get_type() override {return  "ParticleForceCompute";};
    using force_function_t = std::function<ArrayUtilities::Array3D(
            const Particle &)>;
    force_function_t force_function;
    using ForceCompute::ForceCompute;
    ParticleForceCompute(const System &sys, force_function_t force_function)
            : ForceCompute(sys), force_function(force_function) {}
    void compute() override;
};

/**
 * ForceCompute between a pair of Particles
 */
struct PairBondForce : public ForceCompute {
    inline virtual std::string get_type() override {return  "PairBondForce";};
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

// TODO Add BondForce and BondForceCompute given force per bond
// This should compute the force F_ab only once (per bond) and assign the force
// to the particles i.e F_ab = - F_ba
//
struct BondForce {
    Bond* bond; // not owning
    ArrayUtilities::Array3D force;
    BondForce() = default;
    BondForce(Bond * input_bond, const ArrayUtilities::Array3D & input_force) :
      bond(input_bond), force(input_force) {}
    BondForce(const BondForce&) = default;
    BondForce(BondForce&&) = default;
    BondForce& operator=(const BondForce&) = default;
    BondForce& operator=(BondForce&&) = default;
    ~BondForce() = default;
};

struct PairBondForceWithBond : public ForceCompute {
    inline virtual std::string get_type() override {return  "PairBondForceWithBond";};
    using force_function_t = std::function<ArrayUtilities::Array3D(
            const Particle &, const Particle &, const Bond &)>;
    using ForceCompute::ForceCompute;
    PairBondForceWithBond(const System &sys): ForceCompute(sys) {
              bond_forces.reserve(sys.bonds.bonds.size());
              for (const auto &bond : sys.bonds.bonds) {
                bond_forces.emplace_back(bond.get(), ArrayUtilities::Array3D());
              }
    }
    PairBondForceWithBond(const System &sys, force_function_t force_function)
            : PairBondForceWithBond(sys) {
              force_function = force_function;
            }
    force_function_t force_function;
    void compute() override;
    std::vector<BondForce> bond_forces;
    inline void reset_bond_forces_to_zero() {
      for(auto & bf : bond_forces) {
        std::fill(bf.force.begin(), bf.force.end(), 0);
      }
    };

  protected:
    const ParticleNeighborsCollection &conexions = m_sys.conexions;
};

struct FixedPairBondForceWithBond: public PairBondForceWithBond {
  inline virtual std::string get_type() override {return  "FixedPairBondForceWithBond";};
  using PairBondForceWithBond::PairBondForceWithBond;
  void compute() override;
  void compute_once();
  void negate_forces();
  bool forces_are_populated = false;
};

} // namespace SG
#endif
