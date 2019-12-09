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

#ifndef SG_BOND_HPP
#define SG_BOND_HPP
#include <cstddef>
#include <ostream>

namespace SG {

struct Bond {
    // id of bonded particles
    size_t id_a;
    size_t id_b;
    Bond() = default;
    Bond(const size_t &a, const size_t &b) : id_a(a), id_b(b){};
    Bond(const Bond &) = default;
    Bond(Bond &&) = default;
    Bond &operator=(const Bond &) = default;
    Bond &operator=(Bond &&) = default;
    virtual ~Bond() = default;
};

bool operator==(const Bond &lhs, const Bond &rhs);
bool operator!=(const Bond &lhs, const Bond &rhs);
bool operator<(const Bond &lhs, const Bond &rhs);

void print(const Bond &bonded_pair,
           std::ostream &os,
           bool add_end_of_line = true);
// *
// * Ensures id_a to be smaller than id_b in a bonded_pair.
// *
// * @param bonded_pair
//
void sort(Bond &bonded_pair);

struct BondChain : public Bond {
    double length_contour = 1.0;
    BondChain(const size_t &a,
              const size_t &b,
              const double &input_contour_lenght)
            : Bond(a, b), length_contour(input_contour_lenght){};
    using Bond::Bond;
};

} // namespace SG
#endif
