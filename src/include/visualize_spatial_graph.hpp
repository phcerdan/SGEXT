/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VISUALIZE_SPATIAL_GRAPH_HPP
#define VISUALIZE_SPATIAL_GRAPH_HPP
#include "spatial_graph.hpp"

namespace SG {

/**
 * Visualize sg with a vtk graph layout view.
 *
 * @param sg
 */
void visualize_spatial_graph(const GraphType & sg);

} // namespace SG
#endif
