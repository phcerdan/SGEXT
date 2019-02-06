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

#include "filter_spatial_graph.hpp"
#include "spatial_node.hpp"
#include "spatial_edge.hpp"
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/copy.hpp>

namespace SG {
GraphType filter_by_bounding_box(const BoundingBox & box, GraphType & g) {
  // std::vector<SG::PointType> nodes_outside_with_edges_inside;
  using FilteredGraphType = boost::filtered_graph<GraphType,
        std::function<bool(GraphType::edge_descriptor)>,
        // boost::keep_all,
        std::function<bool(GraphType::vertex_descriptor)>>;
        // boost::keep_all>

  auto edge_lambda = [&](GraphType::edge_descriptor ed) {
    auto & edge_points = g[ed].edge_points;
    for(auto & point : edge_points) {
      if(is_inside(point, box)) {
        return true;
      }
    }
    return false;
  };
  auto vertex_lambda = [&](GraphType::vertex_descriptor vd) {
    const auto & pos = g[vd].pos;
    if(is_inside(pos, box)) {
      return true;
    } else { // iterate over the edges to check edge_points are inside or not
      const auto out_edges = boost::out_edges(vd, g);
      for(auto ei = out_edges.first; ei != out_edges.second; ++ei){
        const auto & edge_points = g[*ei].edge_points;
        for(const auto & point : edge_points) {
          if(is_inside(point, box)) {
            // nodes_outside_with_edges_inside.push_back(pos);
            return true;
          }
        }
      }
    }
    return false;
  };
  FilteredGraphType filtered_view(g,
      // Keep edge if any of the edge_points is inside the box
      // {},
      edge_lambda,
      // {}
      vertex_lambda
      );
  GraphType out_filtered_graph;
  boost::copy_graph(filtered_view, out_filtered_graph);
  return out_filtered_graph;
}
} // ns SG
