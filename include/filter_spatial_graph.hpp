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

#ifndef FILTER_SPATIAL_GRAPH_HPP
#define FILTER_SPATIAL_GRAPH_HPP

#include "spatial_graph.hpp"
#include "bounding_box.hpp"

namespace SG {
/**
 * Return a new graph which is inside the bounding box. Please note that indices or ids
 * are unrelated to the input graph.
 *
 * Nodes might exist outside the bounding box if any edge crosses the boundary.
 *
 * @param box the bounding box in the same coordinates than the spatial graph
 * @param g spatial graph
 *
 * @return a copy of the graph
 */
GraphType filter_by_bounding_box(const BoundingBox & box, GraphType & g);
} /* ns SG */
#endif
