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

GraphType::edge_descriptor select_random_edge(const GraphType &graph);

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
 * Cosine director between target edge and a vector of edges.
 * All of these vectors should be pointing outwards if all of them are sharing a
 * vertex.
 *
 * @param outgoing_edges vector of outgoing edges
 * @param outgoing_target_edge edge from where we want to compute the
 * cosine_directors.
 *
 * @return cosine directors between outgoing_edges and outgoing_target_edge
 */
std::vector<double> cosine_directors_between_edges_and_target_edge(
        const std::vector<VectorType> &outgoing_edges,
        const VectorType &outgoing_target_edge);

/**
 * Returns the edge arrays (mathematical vectors)
 * of the adjacent edges of the edge defined from the inputs
 * source and target.
 * The edges are adjacent to the source, the first input
 *
 * @param source
 * @param target
 * @param graph
 *
 * @return
 */
std::vector<VectorType> get_adjacent_edges_from_source(
        const GraphType::vertex_descriptor source,
        const GraphType::vertex_descriptor target,
        const GraphType &graph,
        const ArrayUtilities::boundary_condition &boundary_condition);
/**
 * Compute cosine_directors of all the edges adjacent to source (except
 * the one specied by ignore_node) versus the vector defined by:
 * target_pos - source_pos.
 *
 * For old_cosines computations ignore_node is equal to target, but for
 * new_cosines the graph is still unchanged, so ignore_node corresponds to
 * the target of the old_edge
 *
 * @param source
 * @param ignore_node
 * @param source_pos
 * @param target_pos
 * @param graph
 *
 * @return  vector with cosine_directors values
 */
std::vector<double> compute_cosine_directors_from_source(
        const GraphType::vertex_descriptor source,
        const GraphType::vertex_descriptor ignore_node,
        const PointType source_pos,
        const PointType target_pos,
        const GraphType &graph,
        const ArrayUtilities::boundary_condition &boundary_condition);

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
        const GraphType &graph, const ArrayUtilities::boundary_condition &bc);

std::vector<double> get_all_cosine_directors_between_connected_edges(
        const GraphType &graph, const ArrayUtilities::boundary_condition &bc);
} /* end namespace SG */
#endif
