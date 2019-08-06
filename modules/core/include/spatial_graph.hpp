/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SPATIAL_GRAPH_HPP
#define SPATIAL_GRAPH_HPP
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include "spatial_edge.hpp"
#include "spatial_node.hpp"

namespace SG {
using GraphAL = boost::adjacency_list<boost::listS,
                                      boost::vecS,
                                      boost::undirectedS,
                                      SpatialNode,
                                      SpatialEdge>;
using GraphType = GraphAL;
} // namespace SG
#endif
