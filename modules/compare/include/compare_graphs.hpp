/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
