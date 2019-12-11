/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
void print(const BondChain &bonded_pair,
           std::ostream &os,
           bool add_end_of_line);

} // namespace SG
#endif
