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
