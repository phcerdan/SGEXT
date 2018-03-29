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

#include "trim_graph.hpp"
#include "spatial_histograms.hpp"
#include "compute_graph_properties.hpp"

namespace SG {

// Iterate over all edges of input_sg
// Keep a std::unordered_map<vertex_descriptor, vertex_descriptor>
// between input and output sg
// if source, target of edge are not are already added, add them and the edge.
// This includes SpatialNode and SpatialEdge.
// TODO
GraphType trim_graph(const GraphType & input_sg) {
    auto histo = histogram_degrees(compute_degrees(input_sg));
    return GraphType(0);
}


} // end namespace SG
