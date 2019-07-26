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
