/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef GET_VTK_POINTS_FROM_GRAPH_HPP
#define GET_VTK_POINTS_FROM_GRAPH_HPP

#include "graph_descriptor.hpp"
#include <functional>
#include <string>
#include <vector>
#include <vtkMergePoints.h>
#include <vtkPointLocator.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

namespace SG {


/**
 * Get vtkPoints extracted from the input spatial graph.
 *
 * Returns the points and a map between the points ids and the location
 * of that point in the graph. See @graph_descriptor
 *
 * The geometrical information in the graph is stored in SpatialNode and
 * SpatialEdge of the graph.
 *
 * Returns all the spatial points without any checking for uniqueness.
 *
 * @param g input spatial graph
 *
 * @return points and the map where they are located in the graph
 */
std::pair<
vtkSmartPointer<vtkPoints>,
std::unordered_map< vtkIdType, std::vector<graph_descriptor>>
> get_vtk_points_from_graph(const GraphType & g);

/**
 * Append an inputGraph to the tree structure used to merge points, and update
 * accordingly the map between the vtk points and the set of graphs.
 *
 * @param inputGraph input graph to append
 * @param mergePoints is an existing PointLocator (for example vtkMergePoints
 * from $get_vtk_points_from_graphs)
 * @param unique_id_map map with vtk points ids pointing to different graphs.
 */
void vtk_points_append_graph( const GraphType &inputGraph,
        vtkPointLocator * mergePoints,
        std::unordered_map< vtkIdType, std::vector<graph_descriptor>> & unique_id_map);

/**
 * Returns a unique set of points that are present in any of the inputs graphs
 * (the points might or might not be shared among the graphs), and a map
 * between the points and the graph descriptors.
 * Example:
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(graph0));
    graphs.push_back(std::cref(graph1));
 *
 * @param graphs vector of refrences of graphs
 *
 * @return pair with unique points and the map to the graph descriptors
 * @sa SG::graph_descriptor SG::get_vtk_points_from_graph
 */
std::pair<
vtkSmartPointer<vtkMergePoints>,
std::unordered_map< vtkIdType, std::vector<graph_descriptor>>
> get_vtk_points_from_graphs(const std::vector<std::reference_wrapper<const GraphType>> & graphs);

} // ns SG
#endif
