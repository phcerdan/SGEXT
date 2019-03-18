/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
GraphType trim_graph(const GraphType& input_sg) {
  auto histo = histogram_degrees(compute_degrees(input_sg));
  return GraphType(0);
}

}  // end namespace SG
