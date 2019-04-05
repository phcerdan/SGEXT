/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SPATIAL_GRAPH_UTILITIES_HPP
#define SPATIAL_GRAPH_UTILITIES_HPP

#include "spatial_graph.hpp"
#include "spatial_node.hpp"

namespace SG {

void print_pos(std::ostream &out, const SG::SpatialNode::PointType &pos);

template<typename GraphType>
size_t num_edge_points(const GraphType &sg)
{
  auto edges = boost::edges(sg);
  size_t num_points;
  for(auto ei = edges.first; ei != edges.second; ++ei) {
    for(auto &ep : sg[*ei].edge_points) {
      ++num_points;
    }
  }
  return num_points;
}

template<typename GraphType>
void print_degrees(const GraphType &graph)
{
  std::cout << "Print degrees spatial_graph:" << std::endl;
  std::cout << "Num Vertices: " << boost::num_vertices(graph) << std::endl;
  using vertex_iterator = typename boost::graph_traits<GraphType>::vertex_iterator;
  vertex_iterator vi, vi_end;
  std::tie(vi, vi_end) = boost::vertices(graph);
  for(; vi != vi_end; ++vi) {
    std::cout << *vi << ": " << ArrayUtilities::to_string(graph[*vi].pos)
              << ". Degree: " << boost::out_degree(*vi, graph) << std::endl;
  }
}

template<typename GraphType>
void print_edges(const GraphType &graph)
{
  std::cout << "Print edges spatial_graph:" << std::endl;
  std::cout << "Num Edges: " << boost::num_edges(graph) << std::endl;
  using edge_iterator = typename boost::graph_traits<GraphType>::edge_iterator;
  edge_iterator ei, ei_end;
  std::tie(ei, ei_end) = boost::edges(graph);
  for(; ei != ei_end; ++ei) {
    auto source = boost::source(*ei, graph);
    auto target = boost::target(*ei, graph);
    std::cout << source << "---" << target << " ; ";
    print_pos(std::cout, graph[source].pos);
    std::cout << "---";
    print_pos(std::cout, graph[target].pos);
    std::cout << std::endl;
  }
}

template<typename GraphType>
void print_spatial_edges(const GraphType &graph)
{
  std::cout << "Print edges spatial_graph:" << std::endl;
  std::cout << "Num Edges: " << boost::num_edges(graph) << std::endl;
  using edge_iterator = typename boost::graph_traits<GraphType>::edge_iterator;
  edge_iterator ei, ei_end;
  std::tie(ei, ei_end) = boost::edges(graph);
  for(; ei != ei_end; ++ei) {
    auto source = boost::source(*ei, graph);
    auto target = boost::target(*ei, graph);
    std::cout << source << "---" << target << " ; ";
    print_pos(std::cout, graph[source].pos);
    std::cout << "---";
    print_pos(std::cout, graph[target].pos);
    std::cout << std::endl;
    std::cout << "edge_points: " << graph[*ei].edge_points.size() << std::endl;
    std::cout << graph[*ei] << std::endl;
  }
}

/**
 * Check the graph has unique points
 *
 * @param sg input spatial graph
 *
 * @return repeated_points, true|false
 */
template<typename GraphType>
std::pair<std::set<PointType>, bool> check_unique_points_in_graph(
    const GraphType &sg)
{
  using vertex_descriptor = typename boost::graph_traits<GraphType>::vertex_descriptor;
  using vertex_iterator = typename boost::graph_traits<GraphType>::vertex_iterator;
  using edge_iterator = typename boost::graph_traits<GraphType>::edge_iterator;

  std::set<SG::PointType> unique_points;
  std::set<SG::PointType> repeated_points;
  size_t npoints = 0;
  vertex_iterator vi, vi_end;
  std::tie(vi, vi_end) = boost::vertices(sg);
  for(; vi != vi_end; ++vi) {
    ++npoints;
    auto inserted = unique_points.insert(sg[*vi].pos);
    if(!inserted.second) repeated_points.insert(sg[*vi].pos);
  }

  edge_iterator ei, ei_end;
  std::tie(ei, ei_end) = boost::edges(sg);
  for(; ei != ei_end; ++ei) {
    auto &sg_edge = sg[*ei];
    auto &sg_edge_points = sg_edge.edge_points;
    for(size_t index = 0; index < sg_edge_points.size(); ++index) {
      const auto &p = sg_edge_points[index];
      ++npoints;
      auto inserted = unique_points.insert(p);
      if(!inserted.second) repeated_points.insert(p);
    }
  }

  bool repeated_exists = (unique_points.size() == npoints) ? false : true;
  return std::make_pair(repeated_points, repeated_exists);
}
}  // namespace SG
#endif
