/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bond_collection.hpp"
#include "bond.hpp"
#include <algorithm>

namespace SG {
void BondCollection::sort() {
    std::sort(std::begin(bonds), std::end(bonds),
              [](const std::shared_ptr<Bond> &a,
                 const std::shared_ptr<Bond> &b) { return *a < *b; });
    sorted = true;
};

BondCollection::bond_vector_t::const_iterator
BondCollection::find_bond(const Bond &bond) const {
    return binary_find_bond(std::cbegin(bonds), std::cend(bonds), bond);
}
BondCollection::bond_vector_t::const_iterator
BondCollection::find_bond(size_t bond_id_a, size_t bond_id_b) const {
    return binary_find_bond(std::cbegin(bonds), std::cend(bonds),
                            Bond(bond_id_a, bond_id_b));
};
BondCollection::bond_vector_t
BondCollection::find_all_bonds_with_id(size_t bond_id_any) {
    return find_all_bonds_with_id(std::begin(bonds), std::end(bonds), bond_id_any);
};

void print(const BondCollection &collection,
           std::ostream &os,
           bool each_bond_in_new_line) {
    const bool internal_do_not_add_end_of_line = false;
    for (const auto &bond_ptr : collection.bonds) {
        print(*bond_ptr, os, internal_do_not_add_end_of_line);
        if (each_bond_in_new_line)
            os << std::endl;
        else
            os << ", ";
    }
}

} // namespace SG
