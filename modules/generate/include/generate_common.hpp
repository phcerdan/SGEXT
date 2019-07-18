/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
