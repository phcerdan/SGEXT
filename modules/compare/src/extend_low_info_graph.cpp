/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
