/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bond.hpp"
#include <memory>

namespace SG {

bool operator==(const Bond &lhs, const Bond &rhs) {
    return (lhs.id_a == rhs.id_a) && (lhs.id_b == rhs.id_b);
}
bool operator!=(const Bond &lhs, const Bond &rhs) { return !(lhs == rhs); }
bool operator<(const Bond &lhs, const Bond &rhs) {
    if (lhs.id_a < rhs.id_a) {
        return true;
    }

    if (lhs.id_a == rhs.id_a) {
        if (lhs.id_b < rhs.id_b) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}

void print(const Bond &bonded_pair, std::ostream &os, bool add_end_of_line) {
    os << "[" << bonded_pair.id_a;
    os << ", ";
    os << bonded_pair.id_b;
    os << "]";
    if (add_end_of_line)
        os << std::endl;
};

void sort(Bond &bonded_pair) {
    if (bonded_pair.id_a > bonded_pair.id_b) {
        std::swap(bonded_pair.id_a, bonded_pair.id_b);
    }
};

void print(const BondChain & bonded_pair,
           std::ostream &os,
           bool add_end_of_line) {
    print(static_cast<const Bond&>(bonded_pair), os, false);
    os << " Lc: " << bonded_pair.length_contour;
    if (add_end_of_line)
        os << std::endl;
};
} // namespace SG
