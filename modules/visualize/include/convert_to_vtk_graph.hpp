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

#ifndef CONVERT_TO_VTK_GRAPH_HPP
#define CONVERT_TO_VTK_GRAPH_HPP
#include "spatial_graph.hpp"
#include <vtkMutableUndirectedGraph.h>
#include <vtkSmartPointer.h>

namespace SG {

struct GraphVTK {
  using VertexMap = std::unordered_map<GraphType::vertex_descriptor, vtkIdType>;
  VertexMap vertex_map;
  vtkSmartPointer<vtkMutableUndirectedGraph> vtk_graph =
            vtkSmartPointer<vtkMutableUndirectedGraph>::New();
};

/**
 * Convert spatial graph to a vtk graph. Used for visualization.
 *
 * @param sg input spatial graph
 *
 * @return vtkGraph
 */
GraphVTK
convert_to_vtk_graph(const GraphType &sg, const bool & with_edge_points = false);
} // namespace SG
#endif
