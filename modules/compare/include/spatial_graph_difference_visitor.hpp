/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SPATIAL_GRAPH_DIFFERENCE_VISITOR_HPP
#define SPATIAL_GRAPH_DIFFERENCE_VISITOR_HPP

#include "array_utilities.hpp"
#include "spatial_graph_utilities.hpp"
#include "get_vtk_points_from_graph.hpp"
#include "graph_descriptor.hpp"
#include "graph_points_locator.hpp"
#include "shortest_path.hpp"
#include <algorithm>
#include <boost/graph/adjacency_iterator.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <iostream>
#include <tuple>
#include <vtkOctreePointLocator.h>
#include <vtkIdList.h>

namespace SG {

/**
 *
 * @tparam SpatialGraph
 * @tparam VertexMap
 * @tparam ColorMap
 */
template <typename SpatialGraph, typename VertexMap, typename ColorMap>
struct SpatialGraphDifferenceVisitor : public boost::default_dfs_visitor {
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

  SpatialGraphDifferenceVisitor(SpatialGraph &result_sg,      // result D
                                SpatialGraph &substraend_sg,  // S in D = M - S
                                IdGraphDescriptorMap &point_id_graphs_map,
                                vtkOctreePointLocator *octree, double &radius,
                                ColorMap &color_map, VertexMap &vertex_map,
                                bool &verbose)
      : m_result_sg(result_sg),
        m_substraend_sg(substraend_sg),
        m_point_id_graphs_map(point_id_graphs_map),
        m_octree(octree),
        m_radius(radius),
        m_color_map(color_map),
        m_vertex_map(vertex_map),
        m_verbose(verbose) {}

  /// The resulting graph D
  GraphType m_result_sg;
  /// Graph S in: D = M - S
  GraphType m_substraend_sg;
  /// point id to graph descriptors
  IdGraphDescriptorMap &m_point_id_graphs_map;
  /// point locator, initialized to contain points from M y S
  vtkOctreePointLocator *m_octree;
  /// radius of the sphere used in the octre search for close points
  double &m_radius;
  /// color map to handle which nodes have been visited
  ColorMap &m_color_map;
  /// Map between vertex of input and the resulting graph (m_result_sg)
  VertexMap &m_vertex_map;
  bool &m_verbose;

  /**
   * invoked when a vertex is encountered for the first time.
   *
   * @param u
   * @param input_sg
   */
  void discover_vertex(vertex_descriptor u, const SpatialGraph &input_sg) {
    auto degree = boost::out_degree(u, input_sg);
    if(m_verbose) {
      std::cout << "SpatialGraphDifference. discover_vertex: " << u << " : "
                << ArrayUtilities::to_string(input_sg[u].pos)
                << ". Degree:" << degree << std::endl;
    }
    // Check if current node exist in the result graph already.
    vertex_descriptor sg_vertex_descriptor;
    bool sg_vertex_exists = false;
    auto vertex_search = m_vertex_map.find(u);
    if(vertex_search != m_vertex_map.end()) {
      sg_vertex_exists = true;
      sg_vertex_descriptor = vertex_search->second;
    }
    if(!sg_vertex_exists) {
      auto &input_spatial_node = input_sg[u];
      bool exist_in_substraend_graph;
      graph_descriptor gdesc_substraend_graph;
      std::tie(exist_in_substraend_graph, gdesc_substraend_graph) =
        point_exist_in_substraend_graph(input_spatial_node.pos);
      if(!exist_in_substraend_graph) {
        // TODO: Add extra logic
        sg_vertex_descriptor =
            boost::add_vertex(input_spatial_node, m_result_sg);
        m_vertex_map.emplace(u, sg_vertex_descriptor);
      }
    }
  }

  void examine_edge(edge_descriptor e, const SpatialGraph &input_sg) {
    auto target = boost::target(e, input_sg);
    auto source = boost::source(e, input_sg);

    if(m_verbose) {
      std::cout << "SpatialGraphDifference. examine_edge: " << e
                << " , target: " << target << " : "
                << ArrayUtilities::to_string(input_sg[target].pos) << std::endl;
    }
  }

  /**
   * invoked on each edge as it becomes a member of the edges that form the
   * search tree
   *
   * We might use examine_edge instead to include back_edges.
   * A strategy for that visitor will be to add subtrees of a high
   * info graph that only touch the "result" graph once.
   *
   * @param e
   * @param input_sg
   */
  void tree_edge(edge_descriptor e, const SpatialGraph &input_sg) {
    auto target = boost::target(e, input_sg);
    auto source = boost::source(e, input_sg);

    if(m_verbose) {
      std::cout << "  SpatialGraphDifference. tree_edge: " << e
                << " , target: " << target << " : "
                << ArrayUtilities::to_string(input_sg[target].pos) << std::endl;
    }
  }

  void back_edge(edge_descriptor e, const SpatialGraph &input_sg) {
    auto target = boost::target(e, input_sg);
    auto source = boost::source(e, input_sg);

    if(m_verbose) {
      std::cout << "  SpatialGraphDifference. back_edge: " << e
                << " , target: " << target << " : "
                << ArrayUtilities::to_string(input_sg[target].pos) << std::endl;
    }
  }

private:
  std::pair<bool, graph_descriptor> point_exist_in_substraend_graph(
      const SG::PointType &pos) {
    auto closeIdList = graph_closest_points_by_radius_locator(
        pos, m_octree, m_point_id_graphs_map, m_radius);
    std::vector<IdWithGraphDescriptor> closest_existing_descriptor_by_graph =
        closest_existing_descriptors_by_graph(closeIdList,
                                              m_point_id_graphs_map);
    // the result graph should be the last index in the vector of graphs.
    // The idMap should be constructed with the first index corresponding to the
    // input graph (minuend_sg) and the last index corresponding to
    // substraend_sg
    if(!closest_existing_descriptor_by_graph.back().exist) {
      return std::make_pair(false, graph_descriptor());
    } else {
      return std::make_pair(
          true, closest_existing_descriptor_by_graph.back().descriptor);
    }
  }
};
}  // namespace SG
#endif

