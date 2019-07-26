/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_DEGREE_SEQUENCES_HPP
#define SG_DEGREE_SEQUENCES_HPP

#include <spatial_graph.hpp>
#include <vector>

namespace SG {
std::vector<int> generate_degree_sequence_geometric_distribution_bounded(
        const size_t num_vertices,
        const double &x,
        const size_t min_degree_allowed,
        const size_t max_degree_allowed);

} // namespace SG
#endif
