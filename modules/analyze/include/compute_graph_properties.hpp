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

#ifndef COMPUTE_GRAPH_PROPERTIES_HPP
#define COMPUTE_GRAPH_PROPERTIES_HPP

#include "spatial_graph.hpp"

namespace SG {

/*
 * Compute degree of all nodes
 *
 * @param sg input spatial graph
 *
 * @return vector with degrees
 */
std::vector<unsigned int> compute_degrees(const SG::GraphAL& sg);

/**
 * Compute end to end distances of nodes
 * It iterates over all edges and compute distance(target,source)
 * but filtering out edges with less than \b minimum_size_edges edge_points
 *
 * @param sg input spatial graph
 * @param minimum_size_edges filter out edges with less than this number of
 * points.
 *
 * @return vector with distances.
 */
std::vector<double> compute_ete_distances(const SG::GraphAL& sg,
                                          const size_t minimum_size_edges = 0,
                                          bool ignore_end_nodes = false);

/**
 * Compute contour distances, taking into account every point in the spatial
 * edges. Filtering out edges with less than \b minimum_size_edges edge_points
 *
 * @param sg input spatial graph
 * @param minimum_size_edges filter out edges with less than this number of
 * points.
 *
 * @return vector with distances.
 */
std::vector<double> compute_contour_lengths(const SG::GraphAL& sg,
                                            const size_t minimum_size_edges = 0,
                                            bool ignore_end_nodes = false);

/**
 * Compute angles between adjacent edges in sg
 * but filtering out edges with less than \b minimum_size_edges edge_points
 * and the ability to ignore paralell edges.
 *
 * @param sg input spatial graph
 * @param minimum_size_edges filter out edges with less than this number of
 * points.
 * @param ignore_parallel_edges flag to don't compute angles for parallel edges.
 * if false, parallel edges will have an angle of 0.0.
 *
 * @return  vector with angles.
 */
std::vector<double> compute_angles(const SG::GraphAL& sg,
                                   const size_t minimum_size_edges = 0,
                                   const bool ignore_parallel_edges = false,
                                   const bool ignore_end_nodes = false);

/**
 * Compute std::cos of input angles.
 *
 * @param angles input @sa compute_angles
 *
 * @return vector with cosines of angles
 */
std::vector<double> compute_cosines(const std::vector<double>& angles);
}  // namespace SG
#endif
