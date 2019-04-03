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

#ifndef GET_VTK_POINTS_FROM_GRAPH_HPP
#define GET_VTK_POINTS_FROM_GRAPH_HPP

#include "bounding_box.hpp"
#include "graph_descriptor.hpp"
#include <functional>
#include <string>
#include <vector>
#include <vtkMergePoints.h>
#include <vtkPointLocator.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

namespace SG {

using IdGraphDescriptorMap =
    std::unordered_map<vtkIdType, std::vector<graph_descriptor>>;
using PointsIdMapPair =
    std::pair<vtkSmartPointer<vtkPoints>, IdGraphDescriptorMap>;
using MergePointsIdMapPair =
    std::pair<vtkSmartPointer<vtkMergePoints>, IdGraphDescriptorMap>;

void print_id_graph_descriptor_map(const IdGraphDescriptorMap &);
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
PointsIdMapPair get_vtk_points_from_graph(const GraphType& g);

/**
 * Append an inputGraph to the tree structure used to merge points, and update
 * accordingly the map between the vtk points and the set of graphs.
 *
 * @param inputGraph input graph to append
 * @param mergePoints is an existing PointLocator (for example vtkMergePoints
 * from $get_vtk_points_from_graphs)
 * @param unique_id_map map with vtk points ids pointing to different graphs.
 */

void append_new_graph_points(
    const PointsIdMapPair& new_graph_point_map_pair,
    vtkPointLocator* mergePoints,
    std::unordered_map<vtkIdType, std::vector<graph_descriptor>>&
        unique_id_map);

void append_new_graph_points(
    vtkPoints* new_graph_points,  // should be const, but GetNumberOfPoints is
                                  // not const!
    const std::unordered_map<vtkIdType, std::vector<graph_descriptor>>&
        new_graph_id_map,
    vtkPointLocator* mergePoints,
    std::unordered_map<vtkIdType, std::vector<graph_descriptor>>&
        unique_id_map);
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
MergePointsIdMapPair get_vtk_points_from_graphs(
    const std::vector<std::reference_wrapper<const GraphType>>& graphs,
    const BoundingBox* box = nullptr);

}  // namespace SG
#endif
