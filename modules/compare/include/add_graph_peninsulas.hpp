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

#ifndef ADD_GRAPH_PENINSULAS_HPP
#define ADD_GRAPH_PENINSULAS_HPP

#include "graph_descriptor.hpp"
#include "spatial_graph.hpp"
#include <functional>
#include <unordered_map>
#include <vector>
#include <vtkOctreePointLocator.h>
#include <vtkPointLocator.h>
#include <vtkSmartPointer.h>

namespace SG {

struct AddGraphPeninsulasResult {
  GraphType graph;
  vtkSmartPointer<vtkOctreePointLocator> octree;
};

/**
 * A peninsula is any subgraph from graphs[high_info_graph_index] only contacting the result graph
 * in one point.
 *
 * @param graphs
 * @param high_info_graph_index
 * @param extended_graph_index
 * @param idMap
 * @param octree
 * @param radius_touch
 * @param verbose
 */
AddGraphPeninsulasResult add_graph_peninsulas(
    const std::vector<std::reference_wrapper<const GraphType>>& graphs,
    size_t extended_graph_index,
    size_t high_info_graph_index,
    vtkPointLocator* mergePoints,
    std::unordered_map<vtkIdType, std::vector<graph_descriptor>>& idMap,
    double radius_touch,
    bool verbose);

} // end namespace SG
#endif
