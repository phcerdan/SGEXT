/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SPATIAL_GRAPH_UTILITIES_HPP
#define SPATIAL_GRAPH_UTILITIES_HPP

#include "spatial_graph.hpp"
#include "spatial_node.hpp"

namespace SG {

void print_pos(std::ostream &out, const SG::SpatialNode::PointType &pos);

void print_degrees(const GraphType &graph);

void print_edges(const GraphType &graph);

void print_spatial_edges(const GraphType &graph);

/**
 * Check the graph has unique points
 *
 * @param sg input spatial graph
 *
 * @return repeated_points, true|false
 */
std::pair<std::set<PointType>, bool>
check_unique_points_in_graph(const GraphType & sg);
} //end namespace
#endif
