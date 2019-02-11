/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
  std::vector<graph_descriptor> graph_closest_n_points_locator(const PointType &queryPoint, vtkKdTreePointLocator * kdtree,
      const std::unordered_map<vtkIdType, std::vector<graph_descriptor>> & idMap,
      const int closest_n_points = 5 );

  std::vector<graph_descriptor> graph_closest_points_by_radius_locator(const PointType &queryPoint, vtkKdTreePointLocator * kdtree,
      const std::unordered_map<vtkIdType, std::vector<graph_descriptor>> & idMap,
      double radius);
} // ns SG
#endif
