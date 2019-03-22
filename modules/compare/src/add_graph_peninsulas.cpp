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

#include "add_graph_peninsulas.hpp"
#include "add_graph_peninsulas_visitor.hpp"
#include "graph_points_locator.hpp"
#include <vtkIdList.h>
#include <tuple>  // For std::tie

namespace SG {

GraphType add_graph_peninsulas(
    const std::vector<std::reference_wrapper<const GraphType>>& graphs,
    size_t extended_graph_index,
    size_t high_info_graph_index,
    std::unordered_map<vtkIdType, std::vector<graph_descriptor>>& idMap,
    vtkOctreePointLocator* octree, double radius_touch, bool verbose) {
  // We are going to build on top of the extended graph
  GraphType result_sg = graphs[extended_graph_index];
  using vertex_descriptor = boost::graph_traits<GraphType>::vertex_descriptor;
  using vertex_iterator = boost::graph_traits<GraphType>::vertex_iterator;
  using ColorMap = std::map<vertex_descriptor, boost::default_color_type>;
  ColorMap colorMap;
  using Color = boost::color_traits<ColorMap::mapped_type>;
  boost::associative_property_map<ColorMap> propColorMap(colorMap);
  using VertexMap = std::unordered_map<vertex_descriptor, vertex_descriptor>;
  VertexMap vertex_map;

  const GraphType& input_sg = graphs[high_info_graph_index];

  vertex_iterator vi, vi_end;
  std::tie(vi, vi_end) = boost::vertices(input_sg);
  auto vi_start = vi;
  auto start = *vi;

  // Mark as unvisited (white) all the vertices
  {
    vertex_iterator ui, ui_end;
    for(std::tie(ui, ui_end) = boost::vertices(input_sg); ui != ui_end; ++ui) {
      put(propColorMap, *ui, Color::white());
    }
  }

  AddGraphPeninsulasVisitor<GraphType, VertexMap, ColorMap> vis(
      result_sg, graphs, idMap, octree, radius_touch, colorMap, vertex_map,
      verbose);

  /**
   *
   * Visiting a high graph, the visit will end when contacting a graph
   * point (vertex or edge) from the resulting graph.
   *
   * The dfs visit will be started from all two-degree vertex.
   * And the visit will add nodes and branches to the result graph.
   *
   * @param finish_on_touch_result_graph
   * @param is_not_loop
   * @param propColorMap
   * @param
   *
   * @return
   */
  auto finish_on_touch_result_graph = [&idMap, &octree, &radius_touch,
                                       &propColorMap,
                                       &verbose](vertex_descriptor u,
                                                 const GraphType& g) {
    auto closeIdList = graph_closest_points_by_radius_locator(
        g[u].pos, octree, idMap, radius_touch);
    std::vector<IdWithGraphDescriptor> closest_existing_descriptor_by_graph =
        closest_existing_descriptors_by_graph(closeIdList, idMap);
    // the result graph should be the last index in the vector of graphs.
    if(closest_existing_descriptor_by_graph.back().exist) {
      if(verbose)
        std::cout << "Exited due to finish_on_touch_result_graph" << std::endl;
      put(propColorMap, u, Color::white());
      return true;
    } else {
      return false;  // Do not terminate
    }
  };

  // boost::depth_first_visit(input_sg, start, vis, propColorMap,
  //     finish_on_touch_result_graph);
  for(; vi != vi_end; ++vi) {
    auto degree = boost::out_degree(*vi, input_sg);
    if(degree == 1) {
      start = *vi;
      if(verbose) {
        std::cout << "Visit: start: " << start << " : "
                  << ArrayUtilities::to_string(input_sg[start].pos)
                  << ". Degree: " << degree << std::endl;
      }

      boost::depth_first_visit(input_sg, start, vis, propColorMap,
                               finish_on_touch_result_graph);
    }
  }
  return result_sg;
};

}  // end namespace SG
