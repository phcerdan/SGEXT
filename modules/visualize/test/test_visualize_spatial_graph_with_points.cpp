/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "test_fixtures.hpp"
#include "get_vtk_points_from_graph.hpp"

TEST_F(sg_3D, visualize_spatial_graph_with_points) {
    auto points_map_pair = SG::get_vtk_points_from_graph(g);
    SG::visualize_spatial_graph_with_points(g, points_map_pair.first);
}
