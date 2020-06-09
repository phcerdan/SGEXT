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
