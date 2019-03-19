/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COMPARE_VISITOR_HPP
#define COMPARE_VISITOR_HPP

#include "array_utilities.hpp"
#include <algorithm>
#include <boost/graph/adjacency_iterator.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <iostream>
#include <tuple>

namespace SG {

/**
 * The original idea of this visitor is to create a result (m_sg) graph with the
 * same amount of nodes and edges than the low info graph, but where info from
 * high info graphs is gathered to move the end-points, and extend the edges.
 *
 * The resulting m_sg will be the first step to build the final result,
 * where high information is integrated, but 
 * the graph should be still precise with high
 * confidence.
 *
 * The result will the scaffold where more branches from high info graphs will be added.
 *
 * @tparam SpatialGraph
 * @tparam VertexMap
 * @tparam ColorMap
 */
template <typename SpatialGraph, typename VertexMap, typename ColorMap>
struct ExtendLowInfoGraphVisitor : public boost::default_dfs_visitor {

  using SpatialGraphVertexBundle =
      typename boost::vertex_bundle_type<SpatialGraph>::type;
  using SpatialVertex = SpatialGraphVertexBundle;
  using SpatialGraphEdgeBundle =
      typename boost::edge_bundle_type<SpatialGraph>::type;
  using SpatialEdge = SpatialGraphEdgeBundle;
  using vertex_descriptor =
      typename boost::graph_traits<SpatialGraph>::vertex_descriptor;
  using edge_descriptor =
      typename boost::graph_traits<SpatialGraph>::edge_descriptor;

  ExtendLowInfoGraphVisitor(
      SpatialGraph &sg,
      const std::vector<std::reference_wrapper<const SpatialGraph>> &graphs,
      ColorMap & color_map, VertexMap &vertex_map, bool &verbose)
      : m_sg(sg),
        m_graphs(graphs),
        m_color_map(color_map),
        m_vertex_map(vertex_map),
        m_verbose(verbose) {}

  /// The resulting graph
  SpatialGraph &m_sg;
  /// The array of graphs ordered from low to high info.
  const std::vector<std::reference_wrapper<const SpatialGraph>>& m_graphs;
  /// color map to handle which nodes have been visited
  ColorMap &m_color_map;
  /// vertex_descriptor map between input (low-info) graph and the resulting graph
  VertexMap &m_vertex_map;
  bool &m_verbose;

protected:


};
} // end namespace
#endif

