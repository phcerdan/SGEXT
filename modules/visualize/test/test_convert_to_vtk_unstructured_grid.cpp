/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "test_fixtures.hpp"
#include "convert_to_vtk_unstructured_grid.hpp"

TEST_F(sg_3D, convert_to_vtk_unstructured_grid) {
    auto ugrid = SG::convert_to_vtk_unstructured_grid(g);
    const std::string file_name = "./converted_graph_to_vtk_unstructured_grid.vtu";
    SG::write_vtk_unstructured_grid(ugrid, file_name);
}

TEST_F(sg_3D, convert_to_vtk_unstructured_grid_with_edge_points) {
    const std::string file_name = "./converted_graph_to_vtk_unstructured_grid_with_edge_points.vtu";
    auto ugrid = SG::convert_to_vtk_unstructured_grid_with_edge_points(g);
    SG::write_vtk_unstructured_grid(ugrid, file_name);
}
