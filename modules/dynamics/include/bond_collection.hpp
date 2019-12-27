/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_BOND_COLLECTION_HPP
#define SG_BOND_COLLECTION_HPP

#include "bond.hpp"
#include <algorithm>
#include <iostream> // TODO:remove
#include <memory>
#include <vector>

namespace SG {

struct BondCollection {
    using bond_vector_t = std::vector<std::shared_ptr<Bond>>;
    bond_vector_t bonds;
    bool sorted = false;

    void sort();

    static constexpr auto comparator_bond = [](const std::shared_ptr<Bond> &b,
            const Bond &comp_bond) {
        return *b < comp_bond;
    };
    static constexpr auto comparator_id_a = [](const std::shared_ptr<Bond> &b,
            const size_t &comp_bond_id) {
        return b->id_a < comp_bond_id;
    };
    template <typename ForwardIt>
    inline ForwardIt binary_find_bond_id_a(const ForwardIt &first,
                                           const ForwardIt &last,
                                           size_t input_bond_id) const {
        if (!sorted) {
            throw std::runtime_error("BondCollection not sorted before a find. "
                                     "Call sort() first.");
        }
        const auto lower_bound_search_it =
                std::lower_bound(first, last, input_bond_id, comparator_id_a);
        if (lower_bound_search_it == last ||
            input_bond_id < (*lower_bound_search_it)->id_a) {
            return last;
        }
        return lower_bound_search_it;
    }

    template <typename ForwardIt>
    inline bond_vector_t find_all_bonds_with_id(const ForwardIt &first,
                                        const ForwardIt &last,
                                        size_t input_bond_id_any) const {
        if (!sorted) {
            throw std::runtime_error("BondCollection not sorted before a find. "
                                     "Call sort() first.");
        }
        bond_vector_t output_after_first_id_a;
        auto it_first_id_a =
                binary_find_bond_id_a(first, last, input_bond_id_any);
        // check id_a after the first find
        {
            // lower_bound returns the bond with id_a equal or greater
            // the second comparisson is needed to discard greater values.
            // but binary_find_bond_id_a already returns last if not found
            auto next_it_a = it_first_id_a;
            while (next_it_a != last) {
                // && (*next_it_a)->id_a == input_bond_id_any) {
                output_after_first_id_a.push_back(*next_it_a);
                next_it_a++;
                next_it_a = binary_find_bond_id_a(next_it_a, last,
                                                  input_bond_id_any);
            }
        }
        // check id_b before the first find
        // This also works if it_first_id_a is last, we have to check for id_b
        // in the whole container
        bond_vector_t output_before_first_id_a;
        std::copy_if(first, it_first_id_a,
                     std::back_inserter(output_before_first_id_a),
                     [&input_bond_id_any](const std::shared_ptr<Bond> &bond) {
                         return bond->id_b == input_bond_id_any;
                     });
        // merge to keep the output ordered
        output_before_first_id_a.insert(
                output_before_first_id_a.end(),
                std::make_move_iterator(output_after_first_id_a.begin()),
                std::make_move_iterator(output_after_first_id_a.end()));

        return output_before_first_id_a;
    }

    template <typename ForwardIt>
    inline ForwardIt binary_find_bond(const ForwardIt &first,
                                      const ForwardIt &last,
                                      const Bond &input_bond) const {
        if (!sorted) {
            throw std::runtime_error("BondCollection not sorted before a find. "
                                     "Call sort() first.");
        }
        const auto lower_bound_search_it =
                std::lower_bound(first, last, input_bond, comparator_bond);
        // lower bound return iterator first if the input_bond is smaller
        // than the whole container. Return last if not found
        if (lower_bound_search_it == last ||
            input_bond < *(*lower_bound_search_it)) {
            return last;
        }
        return lower_bound_search_it;
    }

    bond_vector_t::const_iterator find_bond(const Bond &b) const;
    bond_vector_t::const_iterator find_bond(size_t bond_id_a,
                                            size_t bond_id_b) const;
    bond_vector_t find_all_bonds_with_id(size_t bond_id_any);
    bond_vector_t find_all_bonds_with_id(size_t bond_id_any) const;
};

void print(const BondCollection &collection,
           std::ostream &,
           bool each_bond_in_new_line = true);

} // namespace SG
#endif
