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

#include "bond.hpp"

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
    os << "id_a: " << bonded_pair.id_a;
    os << ", ";
    os << "id_b: " << bonded_pair.id_b;
    if (add_end_of_line)
        os << std::endl;
};

void sort(Bond &bonded_pair) {
    if (bonded_pair.id_a > bonded_pair.id_b) {
        std::swap(bonded_pair.id_a, bonded_pair.id_b);
    }
};
} // namespace SG
