/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_DEGREE_SEQUENCES_HPP
#define SG_DEGREE_SEQUENCES_HPP

#include <spatial_graph.hpp>
#include <vector>

namespace SG {
/**
 * Generate a sequence of degrees using a geometric distribution bounded
 * to a min_degree_allowed and max_degree_allowed.
 * Usually min_degree_allowed = 3
 *
 * Also add the option @ref percentage_of_one_degree_nodes, that allows to have
 * a bi-functional distribution. Where the degree = 1 distribution is just a delta.
 * And from min_degree_allowed to max_degree_allowed is
 * a geometric_random_distribution.
 *
 * | %1degree   *
 * |    *          *
 * |                 *
 * |                    *
 *  ------------------------------------ Degree
 *      |       |                  |
 *      1    min_degree        max_degree
 *
 * @param num_vertices size of the vector
 * @param x variable for geometric_random_distribution
 * @param min_degree_allowed geometric_distribution first value
 * @param max_degree_allowed geometric_distribution last allowed value.
 * The max degree generated might still be less than this parameter.
 * @param percentage_of_one_degree_nodes the number of degree 1 nodes is
 * not taken into account for the geometric_random_distribution.
 *
 * @return vector with size @ref num_vertices with the generated degrees
 */
std::vector<int> generate_degree_sequence_geometric_distribution_bounded(
        const size_t num_vertices,
        const double &x,
        const size_t min_degree_allowed,
        const size_t max_degree_allowed,
        const double &percentage_of_one_degree_nodes = 0.);

} // namespace SG
#endif
