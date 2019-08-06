/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "degree_sequences.hpp"
#include "rng.hpp"

namespace SG {

std::vector<int> generate_degree_sequence_geometric_distribution_bounded(
        const size_t num_vertices,
        const double &x,
        const size_t min_degree_allowed,
        const size_t max_degree_allowed) {
    assert(min_degree_allowed < max_degree_allowed);
    std::vector<int> degree_sequence(num_vertices);
    const size_t max_iterations = 100;
    for (auto &degree : degree_sequence) {
        int rand_degree = 0;
        size_t iteration = 0;
        do {
            rand_degree = RNG::random_geometric(x) + min_degree_allowed;
            iteration++;
        } while (rand_degree > max_degree_allowed &&
                 iteration < max_iterations);
        if (iteration >= max_iterations) {
            throw std::domain_error(
                    "generate_degree_sequence_geometric_distribution_bounded "
                    "is not able to generate a random degree with "
                    "max_degree_allowed = " +
                    std::to_string(max_degree_allowed) + ".");
        }
        // All good, set the degree;
        degree = rand_degree;
    }
    return degree_sequence;
}
} // namespace SG
