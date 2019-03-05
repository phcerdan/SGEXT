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

#include "spatial_graph.hpp"
#include "filter_spatial_graph.hpp"
#include "bounding_box.hpp"

namespace SG {

std::pair<EdgeDescriptorUnorderedSet, VertexDescriptorUnorderedSet>
remove_edges_and_nodes_from_high_info_graph(
        const GraphType & g0,
        const GraphType & g1,
        const double radius = 2.0);

GraphType
compare_low_and_high_info_graphs(
    const GraphType & g0,
    const GraphType & g1,
    const double radius = 2.0);
} // end ns SG
