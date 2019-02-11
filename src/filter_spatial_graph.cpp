/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "filter_spatial_graph.hpp"
#include "spatial_node.hpp"
#include "spatial_edge.hpp"
#include <boost/graph/copy.hpp>


namespace SG {

FilteredGraphType filter_by_bounding_box_no_copy(const BoundingBox & box, GraphType & g)
{
  auto edge_lambda = [&](GraphType::edge_descriptor ed) -> bool {
        auto & edge_points = g[ed].edge_points;
        for(auto & point : edge_points) {
          if(is_inside(point, box)) {
            return true;
          }
        }
        return false;
      };
  auto vertex_lambda = [&](GraphType::vertex_descriptor vd) -> bool {
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

  return FilteredGraphType(g, edge_lambda, vertex_lambda);
};

GraphType filter_by_bounding_box(const BoundingBox & box, GraphType & g)
{
  auto filtered_view = filter_by_bounding_box_no_copy(box, g);

  GraphType out_filtered_graph;
  boost::copy_graph(filtered_view, out_filtered_graph);
  return out_filtered_graph;
};

FilteredGraphType filter_by_sets_no_copy(
        const std::unordered_set<GraphType::vertex_descriptor> & remove_nodes,
        const EdgeDescriptorUnorderedSet & remove_edges,
        GraphType & g)
{
    auto edge_lambda = [&](GraphType::edge_descriptor ed) -> bool {
        return !static_cast<bool>(remove_edges.count(ed));
    };
    auto vertex_lambda = [&](GraphType::vertex_descriptor vd) -> bool {
        return !static_cast<bool>(remove_nodes.count(vd));
    };

    return FilteredGraphType(g, edge_lambda, vertex_lambda);
}

GraphType filter_by_sets(
        const std::unordered_set<GraphType::vertex_descriptor> & remove_nodes,
        const EdgeDescriptorUnorderedSet & remove_edges,
        GraphType & g)
{

    auto filtered_view = filter_by_sets_no_copy(remove_nodes, remove_edges, g);
    // Copy the graph -- vertex and edge descriptors are invalidated...
    GraphType out_filtered_graph;
    boost::copy_graph(filtered_view, out_filtered_graph);
    return out_filtered_graph;
};
} // ns SG
