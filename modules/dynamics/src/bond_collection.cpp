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
    return find_all_bonds_with_id(std::begin(bonds), std::end(bonds),
                                  bond_id_any);
};
BondCollection::bond_vector_t
BondCollection::find_all_bonds_with_id(size_t bond_id_any) const {
    return find_all_bonds_with_id(std::begin(bonds), std::end(bonds),
                                  bond_id_any);
};

void print(const BondCollection &collection,
           std::ostream &os,
           bool each_bond_in_new_line) {
    const bool internal_do_not_add_end_of_line = false;
    for (const auto &bond_ptr : collection.bonds) {
        print(*std::static_pointer_cast<BondChain>(bond_ptr), os,
              internal_do_not_add_end_of_line);
        if (each_bond_in_new_line)
            os << std::endl;
        else
            os << ", ";
    }
}

} // namespace SG
