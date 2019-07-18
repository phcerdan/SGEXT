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

#ifndef EXTEND_LOW_INFO_GRAPH_HPP
#define EXTEND_LOW_INFO_GRAPH_HPP

#include "graph_descriptor.hpp"
#include "spatial_graph.hpp"
#include <functional>
#include <unordered_map>
#include <vector>
#include <vtkOctreePointLocator.h>
namespace SG {

GraphType extend_low_info_graph_via_dfs(
        const std::vector<std::reference_wrapper<const GraphType>> &graphs,
        std::unordered_map<vtkIdType, std::vector<graph_descriptor>> &idMap,
        vtkOctreePointLocator *octree,
        double radius,
        bool verbose = false);

} // end namespace SG
#endif
