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
