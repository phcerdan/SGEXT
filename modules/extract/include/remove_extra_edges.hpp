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

#ifndef REMOVE_EXTRA_EDGES_HPP
#define REMOVE_EXTRA_EDGES_HPP

#include "spatial_graph.hpp"

namespace SG {
/**
 * Objects in DGtal with a 26_6 topology consider as adjacent vertices those
 * in diagonals. We are interested in keeping that topology, but it generates
 * spurious edges that confuses further analysis.
 * We remove the "diagonal" edges when there are shorter ones. Better an
 * example:
 *
 * o                  o
 * |\                 |
 * o-o      ----->    o-o
 * |/                 |
 * o                  o
 *
 * We are not interested in the diagonal edges, and better keep the center
 * node as the only 3-degree node.
 *
 * See related tests for further details.
 *
 * @param sg input spatial graph to reduce.
 *
 * @return boolean, true if any edge has been removed
 * false otherwhise.
 */
bool remove_extra_edges(GraphType &sg);

} // namespace SG

#endif
