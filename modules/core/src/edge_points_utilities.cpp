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

#include "edge_points_utilities.hpp"
#include <vector>
#include <algorithm>
#include <sstream>

namespace SG {

double edge_points_length(const SG::SpatialEdge &se) {
  const auto &eps = se.edge_points;
  size_t npoints = eps.size();
  // if empty or only one point, return null distance
  if(npoints < 2) return 0.0;
  double length = 0.0;
  for(size_t i = 1; i != npoints; i++) {
    length += ArrayUtilities::distance(eps[i], eps[i - 1]);
  }
  return length;
}

double contour_length(const SG::GraphType::edge_descriptor e,
                      const SG::GraphType &sg) {
  const auto &se = sg[e];
  const auto &eps = se.edge_points;
  auto source = boost::source(e, sg);
  auto target = boost::target(e, sg);
  if(eps.empty())
    return ArrayUtilities::distance(sg[target].pos, sg[source].pos);
  // Because the graph is unordered, source and target are not guaranteed
  // to be the closer to eps[0] or eps.back() respectively, so we compute
  // distance between source to eps[0] to be sure. If they are not connected,
  // we know that source is connected to eps.back().
  auto dist_source_first = ArrayUtilities::distance(sg[source].pos, eps[0]);
  // Dev: It can happen that eps[0] is connected to both, source and target.
  // But eps.back() is only connected to one end of the spatial edge, see test
  // corner case
  auto dist_target_last = ArrayUtilities::distance(sg[target].pos, eps.back());

  auto dist_source_last = ArrayUtilities::distance(sg[source].pos, eps.back());
  auto dist_target_first = ArrayUtilities::distance(sg[target].pos, eps[0]);

  double dist_to_source = 0.0;
  double dist_to_target = 0.0;
  // This cannot assume that spacing is unity as it does.
  // static auto max_connected_distance = sqrt(3.0) + 2.0 *
  // std::numeric_limits<double>::epsilon();

  if(dist_source_first < dist_source_last &&
     dist_target_last < dist_target_first) {
    dist_to_source = dist_source_first;
    dist_to_target = dist_target_last;
  } else {
    dist_to_source = dist_source_last;
    dist_to_target = dist_target_first;
  }

  // sanity check: this won't throw if edge_points are ordered.
  // but it creates a false positive error when spacing between pos isn't 1.0
  // {
  //     if(dist_to_source > max_connected_distance || dist_to_target >
  //     max_connected_distance ) {
  //         std::stringstream ss;
  //         ss << "Contour_length failure, node positions are not connected to
  //         the edge_points. Or the edge_points are disordered.\n"; ss <<
  //         "Source:    " << sg[source] << " , Target: " << sg[target] << "\n";
  //         ss << "Distances: " << dist_to_source << ", " << dist_to_target <<
  //         "\n"; ss << "SpatialEdge:\n " << se; throw std::runtime_error(
  //         ss.str() );
  //     }
  // }

  return dist_to_source + SG::edge_points_length(se) + dist_to_target;
}

void insert_unique_edge_point_with_distance_order(
    SG::SpatialEdge::PointContainer &edge_points,
    const SG::SpatialEdge::PointType &new_point) {
  if(edge_points.empty()) edge_points.push_back(new_point);
  // Insert it between closer points.
  // Compute distance between in-point and all the points.
  std::vector<double> distances_to_in_point(edge_points.size());
  std::transform(std::begin(edge_points), std::end(edge_points),
                 std::begin(distances_to_in_point),
                 [&new_point](const SG::SpatialEdge::PointType &edge_point) {
                   return ArrayUtilities::distance(edge_point, new_point);
                 });
  // Note: edge_points are contiguous (from DFS)
  // the new pos, should be at the beggining or at the end.
  // If at the beginning, we put the first, if at the end, we put it last.
  // Ordering the edge_points if they get disordered is not trivial at all.
  // Check "spatial data" structures elsewhere.
  auto min_it = std::min_element(std::begin(distances_to_in_point),
                                 std::end(distances_to_in_point));
  // Check they are connected for sanity.
  // TODO we might check this only in debug mode.
  // This check is only valid if the spacing is 1.0 (object/indices space)
  {
    auto min_dist = *min_it;
    if(min_dist > sqrt(3.0) + 2.0 * std::numeric_limits<double>::epsilon())
      throw std::runtime_error(
          "The impossible, new_point in "
          "insert_unique_edge_point_with_distance_order is not connected to "
          "the edge_points");
  }
  // Check that you are not inserting a duplicate
  {
    auto min_dist = *min_it;
    if(std::abs(min_dist) < 2.0 * std::numeric_limits<double>::epsilon()) {
#if !defined(NDEBUG)
      std::cout << "Note: insert_unique_edge_point_with_distance_order tried "
                   "to insert a duplicated point, but it was rejected."
                << std::endl;
      std::cout << ArrayUtilities::to_string(new_point) << std::endl;
#endif
      return;
    }
  }
  auto min_index = std::distance(std::begin(distances_to_in_point), min_it);
  if(min_index == 0)
    edge_points.insert(std::begin(edge_points), new_point);
  else if(static_cast<unsigned int>(min_index) ==
          distances_to_in_point.size() -
              1)  // This is safe, as vector is not empty.
    // edge_points.insert(std::end(edge_points), new_point);
    edge_points.push_back(new_point);
  else  // illogical error
    throw std::runtime_error(
        "The impossible, new point in insert_unique_edge_point_with_distance "
        "is not at the beggining or end position in edge_points.");
}

}  // namespace SG
