/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
