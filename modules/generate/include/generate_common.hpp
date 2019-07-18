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

#ifndef SG_GENERATE_COMMON_HPP
#define SG_GENERATE_COMMON_HPP

#include "boundary_conditions.hpp"
#include "common_types.hpp"
#include "histo.hpp"
#include "spatial_graph.hpp"

namespace SG {

using Histogram = histo::Histo<double, size_t>;
/**
 * Select a random node from the input graph.
 *
 * @param graph
 *
 * @return  the vertex id
 */
GraphType::vertex_descriptor select_random_node(const GraphType &graph);

/**
 * Generate a vector with modulus between 0 and max_modulus and random
 * direction.
 *
 * @param max_modulus
 *
 * @return the random array
 */
PointType generate_random_array(const double &max_modulus);

/**
 * Given a list of VectorTypes (with a direction),
 * compute cosine_directors of pairs of this (without repetition)
 * Given vectors: A B C --> returns cosine director: AB, AC, BC
 *
 * Note that the edges have a direction, it is recommended to create the list of
 * VectorTypes from for example a list of adjacent_vertices from a target node.
 * The outer vectors/edges from the target node would be:
 *  out_end_to_end_edge = pos_neighbor - pos_target_node
 * Also note that boundary conditions could be applied when computing that
 * vector.
 *
 * @param outgoing_edges vector of VectorTypes
 *
 * @return cosine_directors of pairs of input edges
 */
std::vector<double> cosine_directors_from_connected_edges(
        const std::vector<VectorType> &outgoing_edges);

/**
 * Computes the end to end distance of all the edges
 * from input graph.
 *
 * End to end distance implies that edge_points, and any tortuosity of the edges
 * is ignored.
 *
 * @param g input graph
 * @param bc boundary condition because distance involve pair of positions
 *
 * @return vector with all the distances (unordered)
 */
std::vector<double> get_all_end_to_end_distances_of_edges(
        const GraphType &g, const ArrayUtilities::boundary_condition &bc);
} /* end namespace SG */
#endif
