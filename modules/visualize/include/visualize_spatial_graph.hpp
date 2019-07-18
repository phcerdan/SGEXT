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

namespace SG {

vtkSmartPointer<vtkGraphLayoutView>
create_graph_layout_view_from_spatial_graph(const GraphType &sg);
/**
 * Visualize sg with a vtk graph layout view.
 * Use: visualize_spatial_graph->GetInteractor()->Start();
 *
 * @param sg
 */
void visualize_spatial_graph(const GraphType &sg);

void visualize_spatial_graph_with_points(const GraphType &sg,
                                         vtkPoints *points,
                                         const double pointsOpacity = 0.8,
                                         const double lengthX = 1.0,
                                         const double lengthY = 1.0,
                                         const double lengthZ = 1.0);
} // namespace SG
#endif
