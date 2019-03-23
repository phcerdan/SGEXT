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

#include "spatial_graph_difference.hpp"
#include "spatial_graph_difference_visitor.hpp"
#include "graph_points_locator.hpp"
#include <vtkIdList.h>
#include <tuple>  // For std::tie

namespace SG {

GraphType spatial_graph_difference(GraphType& minuend_sg,
                                   GraphType& substraend_sg,
                                   double radius_touch, bool verbose) {
  // We are going to build on top of the extended graph
  GraphType diff_sg;
  using vertex_descriptor = boost::graph_traits<GraphType>::vertex_descriptor;
  using vertex_iterator = boost::graph_traits<GraphType>::vertex_iterator;
  using ColorMap = std::map<vertex_descriptor, boost::default_color_type>;
  ColorMap colorMap;
  using Color = boost::color_traits<ColorMap::mapped_type>;
  boost::associative_property_map<ColorMap> propColorMap(colorMap);
  using VertexMap = std::unordered_map<vertex_descriptor, vertex_descriptor>;
  VertexMap vertex_map;

  vertex_iterator vi, vi_end;
  std::tie(vi, vi_end) = boost::vertices(minuend_sg);
  auto vi_start = vi;
  auto start = *vi;

  // Mark as unvisited (white) all the vertices
  {
    vertex_iterator ui, ui_end;
    for(std::tie(ui, ui_end) = boost::vertices(minuend_sg); ui != ui_end;
        ++ui) {
      put(propColorMap, *ui, Color::white());
    }
  }
  // Create the point locator from input graphs
  std::vector<std::reference_wrapper<const GraphType>> graphs;
  graphs.reserve(2);
  graphs.push_back(std::cref(minuend_sg));
  graphs.push_back(std::cref(substraend_sg));
  auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs);
  auto& idMap = merger_map_pair.second;
  auto octree = SG::build_octree_locator(merger_map_pair.first->GetPoints());

  SpatialGraphDifferenceVisitor<GraphType, VertexMap, ColorMap> vis(
      diff_sg, substraend_sg, idMap, octree, radius_touch, colorMap, vertex_map,
      verbose);

  boost::depth_first_search(minuend_sg, vis, propColorMap);
  return diff_sg;
};  // namespace SG

}  // end namespace SG
