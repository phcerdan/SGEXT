/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SPATIAL_GRAPH_DIFFERENCE_HPP
#define SPATIAL_GRAPH_DIFFERENCE_HPP

#include "graph_descriptor.hpp"
#include "spatial_graph.hpp"
#include <functional>
#include <unordered_map>
#include <vector>

namespace SG {

/**
 * Compute the difference between graphs using their spatial location
 * Returns: D = M - S
 *
 * The difference uses vertex positions. Edge positions are ignored.
 *
 * Nodes with adjacent nodes in M are kept, even if that same node exists in S.
 *
 * @param minuend_sg M in D = M - S
 * @param substraend_sg S in D = M - S
 * @param radius_touch radius used to search for neighbors
 *  in the octree point locator constructed with the two input graphs.
 * @param verbose
 *
 * @return
 */
GraphType spatial_graph_difference(const GraphType &minuend_sg,
                                   const GraphType &substraend_sg,
                                   double radius_touch,
                                   bool verbose);
} // end namespace SG
#endif
