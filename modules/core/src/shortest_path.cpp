/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "shortest_path.hpp"
#include "edge_points_utilities.hpp"
#include <boost/property_map/function_property_map.hpp>

namespace SG {

SpatialEdge create_edge_from_path(
    const std::vector<GraphType::vertex_descriptor> & vertex_path,
    const GraphType & input_g)
{
  SpatialEdge sg_edge;
  auto & sg_edge_points = sg_edge.edge_points;
  using vertex_descriptor = GraphType::vertex_descriptor;
  using edge_descriptor = GraphType::edge_descriptor;
  // Check that vertex_path is connected
  for(size_t index = 1; index < vertex_path.size(); ++index){
    vertex_descriptor target = vertex_path[index];
    vertex_descriptor source = vertex_path[index - 1];
    const auto edge_between = boost::edge(source, target, input_g);
    if(!edge_between.second)
      throw("create_edge_from_path: edge does not exist between consecutive "
          "vertices in the input path");
    const auto ed = edge_between.first;
    const auto & edge_points = input_g[ed].edge_points;
    if(index == 1) {
      sg_edge_points = edge_points;
      continue;
    } else {
      // Add the position of the node into the edge_points
      SG::PointType merge_node_pos = input_g[source].pos;
      SG::insert_unique_edge_point_with_distance_order( sg_edge_points, merge_node_pos);
      // TODO this could be slow... optimization welcome to append the whole vector
      for(const auto & p : edge_points) {
        SG::insert_unique_edge_point_with_distance_order( sg_edge_points, p);
      }
    }
  }
  return sg_edge;
}

std::vector<GraphType::vertex_descriptor> compute_shortest_path(
    GraphType::vertex_descriptor start_vertex,
    GraphType::vertex_descriptor end_vertex, const GraphType &input_g,
    bool verbose) {
  using vertex_descriptor = GraphType::vertex_descriptor;
  using edge_descriptor = GraphType::edge_descriptor;
  using edge_iterator = GraphType::edge_iterator;
  size_t visited = 0;
  std::vector<boost::default_color_type> colors(boost::num_vertices(input_g),
                                                boost::default_color_type{});
  std::vector<vertex_descriptor> _pred(boost::num_vertices(input_g),
                                       input_g.null_vertex());
  std::vector<size_t> _dist(boost::num_vertices(input_g), -1ull);

  auto colormap = colors.data();
  auto predmap = _pred.data();
  auto distmap = _dist.data();

  // From sehe:
  // https://stackoverflow.com/questions/29348724/dijkstra-graph-with-a-table-of-weights-on-each-edge
  auto weight_f = [&input_g](edge_descriptor ed) {
    // auto source = boost::source(ed, input_g);
    // auto target = boost::target(ed, input_g);
    // return ArrayUtilities::distance(input_g[source].pos,
    // input_g[target].pos);
    return SG::contour_length(ed, input_g);
  };

  auto weightmap =
      boost::make_function_property_map<edge_descriptor, double>(weight_f);
  shortest_path_visitor vis(end_vertex, visited);

  try {
    if(verbose)
      std::cout << "Searching from #" << start_vertex << " to #" << end_vertex << "...\n";
    boost::dijkstra_shortest_paths(input_g, start_vertex,
                                   boost::visitor(vis)
                                       .color_map(colormap)
                                       .distance_map(distmap)
                                       .predecessor_map(predmap)
                                       .weight_map(weightmap));
    // .weight_map(boost::make_constant_property<edge_descriptor>(1ul)));
  } catch(shortest_path_visitor::done &) {
    if(verbose)
      std::cout << "Completed. Percentage visited: "
        << (100.0 * visited / boost::num_vertices(input_g)) << "%\n";
  }

  size_t distance = distmap[end_vertex];
  if(verbose)
    std::cout << "Distance from #" << start_vertex << " to #" << end_vertex
      << ": " << distance << "\n";

  std::vector<vertex_descriptor> path_out;
  if(distance != size_t(-1)) {
    std::deque<vertex_descriptor> path;
    for(vertex_descriptor current = end_vertex;
        current != input_g.null_vertex() && predmap[current] != current &&
        current != start_vertex;) {
      path.push_front(predmap[current]);
      current = predmap[current];
    }

    if(verbose) {
      std::cout << "Path from #" << start_vertex << " to #" << end_vertex << ": ";
      std::copy(path.begin(), path.end(),
          std::ostream_iterator<vertex_descriptor>(std::cout, ", "));
      std::cout << end_vertex << "\n";
    }

    std::copy(path.begin(), path.end(), std::back_inserter(path_out));
    path_out.push_back(end_vertex);
  }
  return path_out;
}
}  // end namespace SG
