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

#ifndef GRAPH_POINTS_LOCATOR_HPP
#define GRAPH_POINTS_LOCATOR_HPP

#include "graph_descriptor.hpp"
#include <vtkDataSet.h>
#include <vtkOctreePointLocator.h>
#include <vtkSmartPointer.h>

namespace SG {

struct IdWithGraphDescriptor {
    bool exist = false;
    vtkIdType id;
    graph_descriptor descriptor;
};

/**
 * The output is a vector (of size equal to the number of graphs on input idMap)
 * where each index is associated to a graph, and it contains a graph_descriptor
 * and a point id, corresponding to the closest EXISTING point for that graph.
 *
 * Given an ordered list, return a list with graph_descriptors and point id.
 * The order of filling is given by the the order of the input list.
 * The size of the return vector is given by the number of graphs existing in
 * idMap.
 *
 * @param closeIdList
 * @param idMap
 *
 * @return vector of graph_descriptos with id
 */
std::vector<IdWithGraphDescriptor> closest_existing_descriptors_by_graph(
        vtkIdList *closeIdList,
        const std::unordered_map<vtkIdType, std::vector<graph_descriptor>>
                &idMap);

/**
 * Similar to \ref closest_existing_descriptors_by_graph but returns the closest
 * vertex instead of just the closest existing vertex OR edge.
 *
 * @param closeIdList
 * @param idMap
 *
 * @return
 */
std::vector<IdWithGraphDescriptor> closest_existing_vertex_by_graph(
        vtkIdList *closeIdList,
        const std::unordered_map<vtkIdType, std::vector<graph_descriptor>>
                &idMap);

/**
 * Builds a octree from input points
 *
 * @param inputPoints vtk points extracted from a spatial graph
 *
 * @return the octree point locator
 */
vtkSmartPointer<vtkOctreePointLocator>
build_octree_locator(vtkPoints *inputPoints);

/**
 * False if any gdesc.exist == false;
 *
 * @param gdescs
 *
 * @return false if any gdesc.exist == false
 */
bool all_graph_descriptors_exist(const std::vector<graph_descriptor> &gdescs);
bool all_graph_descriptors_exist(
        const std::vector<IdWithGraphDescriptor> &gdescs);

/**
 * Use the octree point locator and the idMap from a set of graphs to query a
 * point in space. It returns a list of vtkId points
 *
 * @param queryPoint
 * @param octree
 * @param
 */
vtkSmartPointer<vtkIdList> graph_closest_n_points_locator(
        const PointType &queryPoint,
        vtkOctreePointLocator *octree,
        const int closest_n_points = 5);

vtkSmartPointer<vtkIdList> graph_closest_points_by_radius_locator(
        const PointType &queryPoint,
        vtkOctreePointLocator *octree,
        double radius);

} // namespace SG
#endif
