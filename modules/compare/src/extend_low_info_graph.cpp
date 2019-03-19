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

#include "extend_low_info_graph.hpp"
#include "extend_low_info_graph_visitor.hpp"

namespace SG {
GraphType extend_low_info_graph_via_dfs(
    const std::vector<std::reference_wrapper<const GraphType>>& graphs,
    bool verbose) {
  GraphType sg;
  using vertex_descriptor = boost::graph_traits<GraphType>::vertex_descriptor;
  using vertex_iterator = boost::graph_traits<GraphType>::vertex_iterator;

  using ColorMap = std::map<vertex_descriptor, boost::default_color_type>;
  ColorMap colorMap;
  using Color = boost::color_traits<ColorMap::mapped_type>;
  boost::associative_property_map<ColorMap> propColorMap(colorMap);

  using VertexMap = std::unordered_map<vertex_descriptor, vertex_descriptor>;
  VertexMap vertex_map;

  ExtendLowInfoGraphVisitor<GraphType, VertexMap, ColorMap> vis(
      sg, graphs, colorMap, vertex_map, verbose);

  return sg;
};
}  // end namespace SG
