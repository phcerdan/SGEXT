/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef TRIM_GRAPH_HPP
#define TRIM_GRAPH_HPP
#include "spatial_graph.hpp"
namespace SG {

/**
 * Create a new graph with no:
 * degree 0 (removed or isolated vertices)
 * degree 1 (end points)
 * degree 2 : (self-loops)
 *    if coming from @reduce_spatial_graph_via_dfs, degree 2
 *    mark the middle of a self-loop.
 *    But extra checking of being a self-loop would be safer.
 *
 * The trimmed returned graph can be used in mechanical simulations of
 * networks and similar, where the removed vertices won't be as important.
 *
 * @param input_sg after being reduced by @reduce_spatial_graph_via_dfs
 *
 * @return new trimmed graph with no degree < 3
 */
GraphType trim_graph(const GraphType &input_sg);

} // namespace SG
#endif
