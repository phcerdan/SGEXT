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

#include "spatial_graph_utilities.hpp"
#include "spatial_graph.hpp"
#include "spatial_node.hpp"

namespace SG {

void print_pos(std::ostream &out, const SG::SpatialNode::PointType &pos) {
    out.precision(100);
    out << "{";
    for (auto &p : pos) {
        if (p >= 0)
            out << " ";
        out << p << ",";
    }
    out << "}";
}

AdjacentVerticesPositions
get_adjacent_vertices_positions(const GraphType::vertex_descriptor target_node,
                                const GraphType &g) {
    AdjacentVerticesPositions adjacent_vertices_positions;
    const auto neighbours = boost::adjacent_vertices(target_node, g);
    for (auto vd : make_iterator_range(neighbours)) {
        adjacent_vertices_positions.neighbours_descriptors.push_back(vd);
        adjacent_vertices_positions.neighbours_positions.push_back(g[vd].pos);
    }
    return adjacent_vertices_positions;
};

} // namespace SG
