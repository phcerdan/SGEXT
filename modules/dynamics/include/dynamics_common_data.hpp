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

#ifndef SG_DYNAMICS_COMMON_DATA_HPP
#define SG_DYNAMICS_COMMON_DATA_HPP

#include "array_utilities.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

namespace SG {
struct ParticleMaterial {
    double radius = 1.0;
    double volume = 1.0;
    double mass = 1.0;
};
inline void print(const ParticleMaterial & p, std::ostream & os) {
    os << "radius = " << p.radius << std::endl;
    os << "volume = " << p.volume << std::endl;
    os << "mass = " << p.mass << std::endl;
};
struct ParticleDynamicProperties {
    ArrayUtilities::Array3D vel;
    ArrayUtilities::Array3D acc;
    ArrayUtilities::Array3D net_force;
};
inline void print(const ParticleDynamicProperties & p, std::ostream & os) {
    os << "vel = " << ArrayUtilities::to_string(p.vel) << std::endl;
    os << "acc = " << ArrayUtilities::to_string(p.acc) << std::endl;
    os << "net_force = " << ArrayUtilities::to_string(p.net_force) << std::endl;
};
struct Particle {
    size_t id;
    ArrayUtilities::Array3D pos;
    ParticleDynamicProperties dynamics;
    ParticleMaterial material;
};
inline bool operator<(const Particle &lhs, const Particle &rhs) {
    return lhs.id < rhs.id;
}

inline void print_id_pos(const Particle & p, std::ostream & os) {
    os << "id = " << p.id << std::endl;
    os << "pos = " << ArrayUtilities::to_string(p.pos) << std::endl;
}
inline void print(const Particle & p, std::ostream & os) {
    print_id_pos(p, os);
    print(p.dynamics, os);
    print(p.material, os);
};
inline void print_trajectory(const Particle & p, std::ostream & os) {
    print_id_pos(p, os);
    print(p.dynamics, os);
};

struct ParticleNeighbors {
    size_t particle_id;            ///< id of the particle
    std::vector<size_t> neighbors; /// ids of the neighbors of the particle

    ParticleNeighbors() = default;
    explicit ParticleNeighbors(const size_t &id) : particle_id(id){};
    ParticleNeighbors(const size_t &id,
                      const std::vector<size_t> &input_neighbors)
            : particle_id(id), neighbors(input_neighbors){};
    ParticleNeighbors(const ParticleNeighbors &) = default;
    ParticleNeighbors(ParticleNeighbors &&) = default;
    ParticleNeighbors &operator=(const ParticleNeighbors &) = default;
    ParticleNeighbors &operator=(ParticleNeighbors &&) = default;
    ~ParticleNeighbors() = default;
};
inline void print(const ParticleNeighbors & particle_neighbors, std::ostream & os) {
    os << "neighbors of particle: " << particle_neighbors.particle_id << std::endl;
    os << "neighbors: { ";
    for(const auto & neigh : particle_neighbors.neighbors) {
        os << neigh << ", ";
    }
    os << " }" << std::endl;

};

struct ParticleNeighborsCollection {
    std::vector<ParticleNeighbors> collection;
};
inline void print(const ParticleNeighborsCollection & all_neighbors, std::ostream & os) {
    for(const auto & p : all_neighbors.collection) {
        print(p, os);
        os << "--" << std::endl;
    }

}

/**
struct BondedPair {
    // id of bonded particles
    size_t id_a;
    size_t id_b;
};
inline bool operator<(const BondedPair& lhs, const BondedPair& rhs)
{
    if(lhs.id_a < rhs.id_a){
        return true;
    }

    if(lhs.id_a == rhs.id_a){
        if(lhs.id_b < rhs.id_b){
            return true;
        } else {
            return false;
        }
    }
    return false;
}
// *
// * Ensures id_a to be smaller than id_b in a bonded_pair.
// *
// * @param bonded_pair
//
void sort(BondedPair & bonded_pair) {
    if(bonded_pair.id_a > bonded_pair.id_b) {
        std::swap(bonded_pair.id_a, bonded_pair.id_b);
    }
};
*/

/**
 * Simplest ParticleCollection.
 * No multi-threaded, no GPU.
 * For optimizations, have a look at HOOMD ParticleData and friends.
 * That includes spatial optimizations (trees) and multi-threading,
 * including GPU acceleration.
 * NOTE(phcerdan): Too early to optimize, but it would be possible to copy
 * from HOOMD if needed.
 * The main drawback (but I might be wrong) is that HOOMD's ParticleData
 * seems to have a fixed num of particles, at least in MD.
 */
struct ParticleCollection {
    std::vector<Particle> particles;
    bool sorted = false;

    inline void sort() {
        std::sort(std::begin(particles), std::end(particles));
        sorted = true;
    };
    template <typename ForwardIt>
    inline ForwardIt binary_find(const ForwardIt &first,
                                 const ForwardIt &last,
                                 const size_t &id_value) const {
        assert(sorted &&
               "Particles not sorted before binary_find. Call sort() first");
        constexpr auto comp = [](const Particle &p, const size_t &value) {
            return p.id < value;
        };
        return std::lower_bound(first, last, id_value, comp);
        // return (it != last && !comp(*it, id_value) ) ? it : last;
    }

    inline std::vector<Particle>::iterator binary_find(const size_t &id_value) {
        return binary_find(std::begin(particles), std::end(particles),
                           id_value);
    }
    inline std::vector<Particle>::const_iterator
    binary_find(const size_t &id_value) const {
        return binary_find(std::cbegin(particles), std::cend(particles),
                           id_value);
    }

    inline std::pair<std::vector<Particle>::const_iterator, size_t>
    find_particle_and_index(const size_t &id_value) const {
        const auto it = binary_find(id_value);
        if (it != particles.end()) {
            return std::make_pair(it, std::distance(particles.begin(), it));
        } else {
            return std::make_pair(it, std::numeric_limits<size_t>::max());
        }
    }

    inline std::pair<std::vector<Particle>::iterator, size_t>
    find_particle_and_index(const size_t &id_value) {
        const auto it = binary_find(id_value);
        if (it != particles.end()) {
            return std::make_pair(it, std::distance(particles.begin(), it));
        } else {
            return std::make_pair(it, std::numeric_limits<size_t>::max());
        }
    }

    inline size_t find_index(const size_t &id_value) const {
        const auto it = binary_find(id_value);
        if (it != particles.end()) {
            return std::distance(particles.begin(), it);
        } else {
            return std::numeric_limits<size_t>::max();
        }
    }
    friend void print(const ParticleCollection &collection);
};
inline void print_end_collection(const ParticleCollection &collection, std::ostream &os) {
    os << "All " << collection.particles.size() << " particles printed" << std::endl;
}
inline void print_id_pos(const ParticleCollection &collection, std::ostream &os) {
    for (const auto &p : collection.particles) {
        print_id_pos(p, os);
        os << "-------------------------------" << std::endl;
    }
    print_end_collection(collection, os);
}
inline void print(const ParticleCollection &collection, std::ostream &os) {
    for (const auto &p : collection.particles) {
        print(p, os);
        os << "-------------------------------" << std::endl;
    }
    print_end_collection(collection, os);
}

/**
 * System is a catch all structure to perform simulations,
 * Classes might need a reference to it in the constructor.
 */
struct System {
    ParticleCollection all;            ///< all particles
    ParticleNeighborsCollection bonds; ///< fixed bonds between particles
    /** Dynamic neighbors per particle based on positions. */
    ParticleNeighborsCollection collision_neighbor_list;
    // Helpers to get references of data.
    inline ArrayUtilities::Array3D & get_position(size_t index) {
        return all.particles[index].pos;
    }
    inline const ArrayUtilities::Array3D & get_position(size_t index) const {
        return all.particles[index].pos;
    }
    inline ArrayUtilities::Array3D & get_velocity(size_t index) {
        return all.particles[index].dynamics.vel;
    }
    inline const ArrayUtilities::Array3D & get_velocity(size_t index) const {
        return all.particles[index].dynamics.vel;
    }
    inline ArrayUtilities::Array3D & get_acceleration(size_t index) {
        return all.particles[index].dynamics.acc;
    }
    inline const ArrayUtilities::Array3D & get_acceleration(size_t index) const {
        return all.particles[index].dynamics.acc;
    }
    // Return copy of positions
    inline auto all_positions_copy() {
        const auto nparts = all.particles.size();
        std::vector<ArrayUtilities::Array3D> positions(nparts);
        for (unsigned int i = 0; i < nparts; ++i) {
            positions[i] = all.particles[i].pos;
        }
        return positions;
    }
    // Return copy of velocities
    inline auto all_velocities_copy() {
        const auto nparts = all.particles.size();
        std::vector<ArrayUtilities::Array3D> velocities(nparts);
        for (unsigned int i = 0; i < nparts; ++i) {
            velocities[i] = all.particles[i].dynamics.vel;
        }
        return velocities;
    }
    // Return copy of accelerations
    inline auto all_accelerations_copy() {
        const auto nparts = all.particles.size();
        std::vector<ArrayUtilities::Array3D> accelerations(nparts);
        for (unsigned int i = 0; i < nparts; ++i) {
            accelerations[i] = all.particles[i].dynamics.acc;
        }
        return accelerations;
    }
};

} // namespace SG
#endif
