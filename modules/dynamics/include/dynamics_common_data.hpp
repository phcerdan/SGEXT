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

#include <cstddef>
#include <vector>
#include "array_utilities.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>

namespace SG {
    struct Particle {
        size_t id;
        ArrayUtilities::Array3D pos;
    };
    inline bool operator<(const Particle& lhs, const Particle& rhs) {
        return lhs.id < rhs.id;
      }
    struct ConnectedParticles {
        size_t particle_id;
        std::vector<size_t> neighbors ;
    };

    /** BondedPair unused
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
        inline ForwardIt binary_find(
                const ForwardIt &first,
                const ForwardIt &last,
                const size_t &id_value) const {
            assert(sorted && "Particles not sorted before binary_find. Call sort() first");
            constexpr auto comp = [](const Particle &p, const size_t &value) {return p.id < value;};
            return std::lower_bound(first, last, id_value, comp);
            // return (it != last && !comp(*it, id_value) ) ? it : last;
        }

        inline std::vector<Particle>::iterator binary_find(const size_t &id_value) {
            return binary_find(std::begin(particles), std::end(particles), id_value);
        }
        inline std::vector<Particle>::const_iterator binary_find(const size_t &id_value) const {
            return binary_find(std::cbegin(particles), std::cend(particles), id_value);
        }
        friend void print(const ParticleCollection& collection);

    };
    void print(const ParticleCollection & collection) {
        for(const auto & p : collection.particles) {
            std::cout << p.id << ": " << ArrayUtilities::to_string(p.pos) << std::endl;
        }
    }




}// end namespace
#endif
