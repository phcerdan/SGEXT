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

#ifndef FILTER_SPATIAL_GRAPH_HPP
#define FILTER_SPATIAL_GRAPH_HPP

#include "spatial_graph.hpp"
#include "spatial_node.hpp"
#include "spatial_edge.hpp"
#include "bounding_box.hpp"

#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/copy.hpp>

// Create a filtered_graph type, use keep tag
// Create bool function for edges and vertices to filter. Return true if in bounding box.
// Function to apply this to a graph given a bounding box.
//  -- With option to just view (the default)
//  -- Or to create a  new graph from the view. This has the problem that the ids
//  of the filtered graph and the original graph might differ.
//
//  The bounding box should be in index or physical coordinates space?
//  Extras needed: A map with keys in bounding box coordinates and values in
//  graph ids, edge and/or vertex.

namespace SG {
/**
 * Return a new graph which is inside the bounding box. Please note that indices or ids
 * are unrelated to the input graph.
 *
 * Nodes might exist outside the bounding box if any edge crosses the boundary.
 *
 * @param box the bounding box in the same coordinates than the spatial graph
 * @param g spatial graph
 *
 * @return a copy of the graph
 */
GraphType filter_by_bounding_box(const BoundingBox & box, GraphType & g)
{

  // std::vector<SG::PointType> nodes_outside_with_edges_inside;
  boost::filtered_graph<GraphType,
    std::function<bool(GraphType::edge_descriptor)>,
    // boost::keep_all,
    std::function<bool(GraphType::vertex_descriptor)>>
    // boost::keep_all>
  filtered_view(g,
      // Keep edge if any of the edge_points is inside the box
      // {},
      [&](GraphType::edge_descriptor ed) {
        auto & edge_points = g[ed].edge_points;
        for(auto & point : edge_points) {
          if(is_inside(point, box)) {
            return true;
          }
        }
        return false;
      },
      // {}
      [&](GraphType::vertex_descriptor vd) {
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
      });
  GraphType out_filtered_graph;
  boost::copy_graph(filtered_view, out_filtered_graph);
  return out_filtered_graph;
}
} /* ns SG */
#endif
