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

#include "shortest_path.hpp"
#include "edge_points_utilities.hpp"
#include <boost/property_map/function_property_map.hpp>

namespace SG {

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
