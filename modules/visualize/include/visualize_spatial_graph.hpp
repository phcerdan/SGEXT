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

#ifndef VISUALIZE_SPATIAL_GRAPH_HPP
#define VISUALIZE_SPATIAL_GRAPH_HPP
#include "spatial_graph.hpp"
#include <vtkGraphLayoutView.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

namespace SG {
namespace defaults {
const std::string graph_layout_win_title = "SGEXT Graph";
const size_t graph_layout_win_width = 600;
const size_t graph_layout_win_height = 600;
} // namespace defaults

/**
 * Create a graph layout in VTK with the input spatial graph.
 * The layout owns a window, a rendered, and an interactor.
 * Use: graph_layout->GetInteractor()->Start();
 * For simple rendering use the layout, @sa visualize_spatial_graph
 *
 * @param sg input spatial graph
 * @param winTitle title of VTK window
 * @param winWidth width of VTK window
 * @param winHeight height of VTK window
 *
 * @return graph layout
 */
vtkSmartPointer<vtkGraphLayoutView> create_graph_layout_view_from_spatial_graph(
        const GraphType &sg,
        const std::string &winTitle = defaults::graph_layout_win_title,
        const size_t &winWidth = defaults::graph_layout_win_width,
        const size_t &winHeight = defaults::graph_layout_win_width);
/**
 * Visualize sg with a vtk graph layout view.
 * @sa create_graph_layout_view_from_spatial_graph
 *
 * @param sg input spatial graph
 * @param winTitle title of VTK window
 * @param winWidth width of VTK window
 * @param winHeight height of VTK window
 */
void visualize_spatial_graph(
        const GraphType &sg,
        const std::string &winTitle = defaults::graph_layout_win_title,
        const size_t &winWidth = defaults::graph_layout_win_width,
        const size_t &winHeight = defaults::graph_layout_win_width);

/**
 * Visualize the nodes of the spatial graph, but with extra points
 * corresponding to the edge points of the spatial graph. These points
 * are represented as cubes with certain pointsOpacity and cube_length
 *
 * @param sg input spatial graph
 * @param points VTK extra points to visualize.
 * @param pointsOpacity alpha of the cubes
 * @param cube_length size of the cube. Useful if spacing in the image is not
 * the default or not homogenous.
 * @param winTitle title of VTK window
 * @param winWidth width of VTK window
 * @param winHeight height of VTK window
 */
void visualize_spatial_graph_with_points(
        const GraphType &sg,
        vtkPoints *points,
        const double pointsOpacity = 0.8,
        const std::array<double, 3> cube_length = {{1.0, 1.0, 1.0}},
        const std::string &winTitle = defaults::graph_layout_win_title,
        const size_t &winWidth = defaults::graph_layout_win_width,
        const size_t &winHeight = defaults::graph_layout_win_width);
} // namespace SG
#endif
