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
#include <vtkKdTreePointLocator.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

namespace SG {

  namespace detail {
  /**
   * Utility function re-used in other function to
   * fill the the graph descriptors with the closest graph_descriptor that exists.
   * The order of filling is determined by closeIdList.
   *
   * This can generate a vector of graph_descriptors with different position.
   *
   * @param in_out_gdescs
   * @param closeIdList
   * @param idMap
   */
  void fill_graph_descriptors(std::vector<graph_descriptor> & in_out_gdescs,
      vtkIdList * closeIdList,
      const std::unordered_map<vtkIdType, std::vector<graph_descriptor>> & idMap);
  } //ns detail

  /**
   * Builds a kdtree from input points
   *
   * @param inputPoints vtk points extracted from a spatial graph
   *
   * @return the kdtree point locator
   */
  vtkSmartPointer<vtkKdTreePointLocator> build_kdtree_locator(vtkPoints * inputPoints);
  /**
   * False if any gdesc.exist == false;
   *
   * @param gdescs
   *
   * @return false if any gdesc.exist == false
   */
  bool all_graph_descriptors_exist(const std::vector<graph_descriptor> & gdescs);

  /**
   * Use the kdtree point locator and the idMap from a set of graphs to query a point in space.
   * It returns a vector of graph_descriptors.
   *
   * @param queryPoint
   * @param kdtree
   * @param
   */
  std::vector<graph_descriptor> graph_closest_n_points_locator(
      const PointType &queryPoint,
      vtkKdTreePointLocator * kdtree,
      const std::unordered_map<vtkIdType, std::vector<graph_descriptor>> & idMap,
      const int closest_n_points = 5 );

  std::vector<graph_descriptor> graph_closest_points_by_radius_locator(
      const PointType &queryPoint,
      vtkKdTreePointLocator * kdtree,
      const std::unordered_map<vtkIdType, std::vector<graph_descriptor>> & idMap,
      double radius);
} // ns SG
#endif
