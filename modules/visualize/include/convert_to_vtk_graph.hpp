/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
convert_to_vtk_graph(const GraphType &sg);
} // namespace SG
#endif
