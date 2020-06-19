/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef GRAPH_DESCRIPTOR_HPP
#define GRAPH_DESCRIPTOR_HPP

#include "spatial_graph.hpp"

namespace SG {

/**
 * Structure holding GraphType descriptors to localize a geometrical point in
 * the graph. The point can be in a node: vertex_descriptor, or in an edge:
 * edge_d + edge_points_index This graph_descriptor is used to map a vtk point
 * to differents graphs.
 */
struct graph_descriptor {
    /** the point exist in the graph */
    bool exist = false;
    /** the point is in an edge of the graph */
    bool is_edge = false;
    /** the point is in a vertex of the graph */
    bool is_vertex = false;
    /** vertex_descriptor where point is located */
    GraphType::vertex_descriptor vertex_d;
    /** edge_descriptor where point is located */
    GraphType::edge_descriptor edge_d;
    /** index of the edge_points vector where point is located inside the edge
     */
    std::size_t edge_points_index = std::numeric_limits<size_t>::max();
};

inline void
print_graph_descriptor(const graph_descriptor &descriptor,
                       const std::string &label = "graph_descriptor",
                       std::ostream &os = std::cout) {
    os << label << ":" << std::endl;
    if (!descriptor.exist) {
        os << " exist: NO" << std::endl;
        return;
    }
    if (descriptor.is_vertex) {
        os << " is_vertex" << std::endl;
        os << " vertex_d: " << descriptor.vertex_d << std::endl;
    }
    if (descriptor.is_edge) {
        os << " is_edge" << std::endl;
        os << " edge_d: " << descriptor.edge_d << std::endl;
        os << " edge_points_index: " << descriptor.edge_points_index
           << std::endl;
    }
    if (descriptor.is_vertex && descriptor.is_edge) {
        os << " WARNING: graph_descriptor is_vertex and is_edge at the same "
              "time."
           << std::endl;
    }
}

} // namespace SG
#endif
