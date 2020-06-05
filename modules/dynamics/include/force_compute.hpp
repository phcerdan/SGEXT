/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_FORCE_COMPUTE_HPP
#define SG_FORCE_COMPUTE_HPP

#include "system.hpp"
#include <functional>
#include <set>

namespace SG {
struct ParticleForce {
    size_t particle_id;
    ArrayUtilities::Array3D force;
    ParticleForce() = default;
    ParticleForce(const size_t &input_particle_id,
                  const ArrayUtilities::Array3D &input_force)
            : particle_id(input_particle_id), force(input_force) {}
    ParticleForce(const ParticleForce &) = default;
    ParticleForce(ParticleForce &&) = default;
    ParticleForce &operator=(const ParticleForce &) = default;
    ParticleForce &operator=(ParticleForce &&) = default;
    ~ParticleForce() = default;
};
/**
 * Force per particle
 */
struct ForceCompute {
    std::vector<ParticleForce> particle_forces;

    explicit ForceCompute(const System *sys) : m_sys(sys) {
        particle_forces.reserve(sys->all.particles.size());
        for (const auto &p : sys->all.particles) {
            particle_forces.emplace_back(p.id, ArrayUtilities::Array3D());
        }
    };
    virtual ~ForceCompute(){};
    // Compute and populate force;
    virtual void compute() = 0;
    inline void reset_forces_to_zero() {
        for (auto &pf : particle_forces) {
            std::fill(pf.force.begin(), pf.force.end(), 0);
        }
    }
    inline virtual std::string get_type() { return "ForceCompute"; };

  protected:
    const System *m_sys;
};

struct ParticleForceCompute : public ForceCompute {
    using force_function_t =
            std::function<ArrayUtilities::Array3D(const Particle &)>;
    force_function_t force_function;

    using ForceCompute::ForceCompute;
    ParticleForceCompute(const System *sys, force_function_t in_force_function)
            : ForceCompute(sys), force_function(in_force_function) {}

    void compute() override;
    inline virtual std::string get_type() override {
        return "ParticleForceCompute";
    };
};

struct ParticleRandomForceCompute : public ParticleForceCompute {
    using ParticleForceCompute::ParticleForceCompute;
    /**
     * Initialize the force_function to a random
     * force that can be used in langevin and brownian
     * dynamics.
     *
     * The modulo of the force is:
     * |Force| = sqrt(2 * dimension * kT * gamma / deltaT)
     * where dimension is 3 by default.
     *
     * @param sys system (for base class)
     * @param kT temperature
     * @param gamma drag coefficient
     * @param deltaT time step
     */
    ParticleRandomForceCompute(const System *sys,
                               const double &kT,
                               const double &gamma /* drag */,
                               const double &deltaT,
                               const size_t &dimension = 3);

    inline virtual std::string get_type() override {
        return "ParticleRandomForceCompute";
    };

    /** These variables can only be set via constructor. */
    /** Temperature */
    const double kT = 0.0;
    /** Drag coefficient. */
    const double gamma = 0.0;
    /** Time step */
    const double deltaT = 0.0;
    /** Dimension of the system. */
    const size_t dimension = 3;
    /** The internal value used for computing the force.
     * Computed using the constructor variables.
     * _modulo = sqrt(2 * dimension * kT * gamma / deltaT)
     * */
    const double _modulo = 0;
};

/**
 * Structure used to store BondForces (instead of particle forces) in
 * PairBondForce and derived classes.
 */
struct BondForce {
    Bond *bond; // not owning
    ArrayUtilities::Array3D force;
    BondForce() = default;
    BondForce(Bond *input_bond, const ArrayUtilities::Array3D &input_force)
            : bond(input_bond), force(input_force) {}
    BondForce(const BondForce &) = default;
    BondForce(BondForce &&) = default;
    BondForce &operator=(const BondForce &) = default;
    BondForce &operator=(BondForce &&) = default;
    ~BondForce() = default;
};

/**
 * PairBondForce is a ForceCompute that computes the force F_ab only once
 * (per bond) and assign the force to the two particles i.e F_ab = - F_ba
 */
struct PairBondForce : public ForceCompute {
    using force_function_t = std::function<ArrayUtilities::Array3D(
            const Particle &, const Particle &, const Bond &)>;
    force_function_t force_function;
    std::vector<BondForce> bond_forces;

    using ForceCompute::ForceCompute;

    /**
     * Only apply the force if the bonds share any tag with @ref in_tags.
     *
     * @param sys shared system
     * @param in_tags container with tags
     */
    void only_apply_to_bonds_with_tags(const System *sys,
                                       const BondProperties::tags_t &in_tags);

    PairBondForce(const System *sys) : ForceCompute(sys) {
        bond_forces.reserve(sys->bonds.bonds.size());
        for (const auto &bond : sys->bonds.bonds) {
            bond_forces.emplace_back(bond.get(), ArrayUtilities::Array3D());
        }
    }
    PairBondForce(const System *sys,
                  const BondProperties::tags_t &in_only_apply_to_bond_with_tags)
            : ForceCompute(sys) {
              only_apply_to_bonds_with_tags(sys, in_only_apply_to_bond_with_tags);
    }

    PairBondForce(const System *sys, force_function_t in_force_function)
            : PairBondForce(sys) {
        force_function = in_force_function;
    }

    PairBondForce(const System *sys,
        force_function_t in_force_function,
        const BondProperties::tags_t &in_only_apply_to_bond_with_tags)
            : PairBondForce(sys, in_only_apply_to_bond_with_tags) {
        force_function = in_force_function;
    }

    void compute() override;
    inline void reset_bond_forces_to_zero() {
        for (auto &bf : bond_forces) {
            std::fill(bf.force.begin(), bf.force.end(), 0);
        }
    };

    inline virtual std::string get_type() override {
        return "PairBondForce";
    };
};

struct FixedPairBondForce : public PairBondForce {
    bool forces_are_populated = false;

    using PairBondForce::PairBondForce;
    void compute() override;
    void compute_once();
    void negate_forces();
    inline virtual std::string get_type() override {
        return "FixedPairBondForce";
    };
};


} // namespace SG
#endif
