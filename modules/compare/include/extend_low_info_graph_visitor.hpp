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

#ifndef COMPARE_VISITOR_HPP
#define COMPARE_VISITOR_HPP

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
 * The original idea of this visitor is to create a result (m_sg) graph with the
 * same amount of nodes and edges than the low info graph, but where info from
 * high info graphs is gathered to move the end-points, and extend the edges.
 *
 * The resulting m_sg will be the first step to build the final result,
 * where high information is integrated, but
 * the graph should be still precise with high
 * confidence.
 *
 * The result will the scaffold where more branches from high info graphs will
 * be added.
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
  using ResultToOriginalVertexMap =
    std::unordered_map<vertex_descriptor,
     std::vector<vertex_descriptor>>;


  ExtendLowInfoGraphVisitor(
      SpatialGraph &sg,
      const std::vector<std::reference_wrapper<const SpatialGraph>> &graphs,
      IdGraphDescriptorMap & point_id_graphs_map, vtkOctreePointLocator* octree,
      ColorMap &color_map, VertexMap &vertex_map, bool &verbose)
      : m_sg(sg),
        m_graphs(graphs),
        m_point_id_graphs_map(point_id_graphs_map),
        m_octree(octree),
        m_color_map(color_map),
        m_vertex_map(vertex_map),
        m_verbose(verbose) {}

  /// The resulting graph
  SpatialGraph &m_sg;
  /// The array of graphs ordered from low to high info.
  const std::vector<std::reference_wrapper<const SpatialGraph>> &m_graphs;
  IdGraphDescriptorMap & m_point_id_graphs_map;
  vtkOctreePointLocator* m_octree;
  /// color map to handle which nodes have been visited
  ColorMap &m_color_map;
  /// vertex_descriptor map between input (low-info) graph and the resulting
  /// graph
  VertexMap &m_vertex_map;
  ResultToOriginalVertexMap m_result_to_original_vertex_map;
  bool &m_verbose;

  /**
   * invoked when a vertex is encountered for the first time.
   *
   * @param u
   * @param input_sg
   */
  void discover_vertex(vertex_descriptor u, const SpatialGraph &input_sg) {
    auto degree = boost::out_degree(u, input_sg);
    if(m_verbose)
      std::cout << "discover_vertex: " << u << " : "
                << ArrayUtilities::to_string(input_sg[u].pos)
                << ". Degree:" << degree << std::endl;
  }

  /**
   * invoked on each edge as it becomes a member of the edges that form the search tree.
   *
   * @param e
   * @param input_sg
   */
  void tree_edge(edge_descriptor e, const SpatialGraph &input_sg) {
    auto target = boost::target(e, input_sg);
    auto source = boost::source(e, input_sg);

    if(m_verbose)
      std::cout << "tree_edge: " << e << " , target: " << target << " : "
                << ArrayUtilities::to_string(input_sg[target].pos) << std::endl;

    std::array<vertex_descriptor, 2> nodes = {source, target};
    std::array<vertex_descriptor, 2> sg_nodes;
    for(size_t source_or_target_index = 0; source_or_target_index < nodes.size();
        ++source_or_target_index ) {
      const auto & u = nodes[source_or_target_index];
      std::cout << "PROCESSING low info u: " << u << std::endl;
      // Check if vertex has been added to the m_sg (Check VertexMap)
      vertex_descriptor sg_vertex_descriptor;
      bool sg_vertex_exists = false;
      auto vertex_search = m_vertex_map.find(u);
      if(vertex_search != m_vertex_map.end()) {
        sg_vertex_exists = true;
        sg_vertex_descriptor = vertex_search->second;
      }
      if(sg_vertex_exists){
        std::cout << "ALREADY ADDED u->m_vertex_map[u]: "
          << u <<"->" <<m_vertex_map[u] << std::endl;
        sg_nodes[source_or_target_index] = m_vertex_map[u];
        continue;  // already added
      }
      // Find the vertices in the other graphs associated to this vertex
      // If there is no vertex, associate it to the source or target of the
      // edge.
      // TODO radius should be a external parameter
      double radius = 4.0;
      auto closeIdList = graph_closest_points_by_radius_locator(
          input_sg[u].pos, m_octree, m_point_id_graphs_map, radius);
      std::vector<IdWithGraphDescriptor> closest_existing_descriptor_by_graph =
          closest_existing_descriptors_by_graph(closeIdList, m_point_id_graphs_map);
      // Get the closest vertex and compare with closest descriptor to check
      // if the vertex has just moved a little bit.
      std::vector<IdWithGraphDescriptor> closest_existing_vert_by_graph =
          closest_existing_vertex_by_graph(closeIdList, m_point_id_graphs_map);

      bool vertex_exists_in_high_info_graphs = true;
      bool vertex_exists_close_by_in_high_info_graphs = false;
      // skip low info graph
      for(size_t index = 1; index < closest_existing_descriptor_by_graph.size();
          ++index) {
        const auto &close_desc = closest_existing_descriptor_by_graph[index];
        if(close_desc.exist) {
          // if the closest descriptor is a vertex
          if(!(close_desc.descriptor.exist && close_desc.descriptor.is_vertex)) {
            vertex_exists_in_high_info_graphs = false;
            // Check if there is a vertex nearby
            const auto & vertex_desc = closest_existing_vert_by_graph[index];
            if(vertex_desc.exist) {
              const auto & vertex_desc_position =
                m_graphs[index].get()[vertex_desc.descriptor.vertex_d].pos;
              const auto & edge_point_position =
                m_graphs[index].get()[close_desc.descriptor.edge_d].edge_points[close_desc.descriptor.edge_points_index];
              auto dist_closest_vertex_and_edge_points =
                ArrayUtilities::distance( vertex_desc_position, edge_point_position);
              // TODO this should be an external parameter and depends on the
              // spacing(positions) of the graph (if any)
              double distance_threshold = 2; // Use 2 voxels radius?
              if(dist_closest_vertex_and_edge_points <= distance_threshold) {
                vertex_exists_close_by_in_high_info_graphs = true;
                vertex_exists_in_high_info_graphs = true;
              } else {
                vertex_exists_close_by_in_high_info_graphs = false;
                vertex_exists_in_high_info_graphs = false;
              }
            }
          }
        } else { // close_desc does not exist
          std::ostringstream os;
          os << "ERROR: vertex_descriptor " << u << " in low info graph, with pos ";
          print_pos(os, input_sg[u].pos);
          os << std::endl;
          os << "Cannot be linked to any point in the high info graph.\n"
            "Radius used: "<<  radius <<". You might try to increase the radius." << std::endl;
          throw std::runtime_error(os.str());
        }
      }

      // TODO deal with differences between high info graphs in the future
      // For now,  do the analysis only in the first high info graph: m_graphs[1].get()
      assert(closest_existing_descriptor_by_graph.size() == 2);
      auto &close_desc = closest_existing_descriptor_by_graph[1];
      auto &close_vertex_desc = closest_existing_vert_by_graph[1];
      std::cout << "vertex_descriptor of input graph: " << u << std::endl;
      std::cout << "vertex_exists_in_high_info_graphs: " << vertex_exists_in_high_info_graphs << std::endl;
      std::cout << "vertex_exists_close_by_in_high_info_graphs: " << vertex_exists_close_by_in_high_info_graphs << std::endl;
      print_graph_descriptor(close_desc.descriptor, "gdesc high info graph: " +
          std::to_string(1));
      print_graph_descriptor(close_vertex_desc.descriptor, "gdesc (vertex) high info graph: " +
          std::to_string(1));
      if(vertex_exists_in_high_info_graphs) {
        std::cout << u << " IS A VERTEX in high graph" << std::endl;
        // Add node from high info graph to the resulting graph
        vertex_descriptor sg_vertex_descriptor;
        if(!vertex_exists_close_by_in_high_info_graphs) {
          sg_vertex_descriptor = boost::add_vertex(
            m_graphs[1].get()[close_desc.descriptor.vertex_d], this->m_sg);
          add_to_result_to_original_map(sg_vertex_descriptor, close_desc.descriptor.vertex_d, 1);
        } else {
          sg_vertex_descriptor = boost::add_vertex(
            m_graphs[1].get()[close_vertex_desc.descriptor.vertex_d], this->m_sg);
          add_to_result_to_original_map(sg_vertex_descriptor, close_vertex_desc.descriptor.vertex_d, 1);
        }
        m_vertex_map.emplace(u, sg_vertex_descriptor);
        sg_nodes[source_or_target_index] = sg_vertex_descriptor;
        add_to_result_to_original_map(sg_vertex_descriptor, u, 0);
      } else { // low info vertex is an edge in high info graph
        std::cout << u << " IS AN EDGE in high graph" << std::endl;
        // Add the target or source of that edge as the vertex.
        // How to choose between source or target?
        // Choose target for now! TODO: BUG, choose wisely instead.
        if(!(close_desc.exist && close_desc.descriptor.is_edge)) {
          std::cout << "WARNING: point in low graph has no correspondece in high graph" << std::endl;
          return;
        }
        auto high_source = boost::source(close_desc.descriptor.edge_d, m_graphs[1].get());
        auto high_target = boost::target(close_desc.descriptor.edge_d, m_graphs[1].get());
        std::cout << "high_source: " << high_source << std::endl;
        std::cout << "high_target: " << high_target << std::endl;
        // TODO perform some logic to choose between source and target.
        // Distances?
        // For now, choose target
        auto sg_vertex_descriptor = boost::add_vertex(
            m_graphs[1].get()[high_target],
            this->m_sg);
        m_vertex_map.emplace(u, sg_vertex_descriptor);
        sg_nodes[source_or_target_index] = sg_vertex_descriptor;
        add_to_result_to_original_map(sg_vertex_descriptor, u, 0);
        add_to_result_to_original_map(sg_vertex_descriptor, sg_vertex_descriptor, 1);
      }
    }
    std::cout <<"source: nodes[0]: " << nodes[0] << std::endl;
    std::cout <<"target: nodes[1]: " << nodes[1] << std::endl;
    std::cout <<"source: sg_nodes[0]: " << sg_nodes[0] << std::endl;
    std::cout <<"target: sg_nodes[1]: " << sg_nodes[1] << std::endl;
    std::cout <<"source: sg_nodes[0]: " << sg_nodes[0] <<
      " -> g0: " << m_result_to_original_vertex_map[sg_nodes[0]][0] <<
      "; g1: " << m_result_to_original_vertex_map[sg_nodes[0]][1] << std::endl;
    std::cout <<"source: sg_nodes[1]: " << sg_nodes[1] <<
      " -> g0: " << m_result_to_original_vertex_map[sg_nodes[1]][0] <<
      "; g1: " << m_result_to_original_vertex_map[sg_nodes[1]][1] << std::endl;
    std::cout << "Adding edges..." << std::endl;

    // Add edges, use shortest_path get the path from sg_nodes[0] to sg_nodes[1]
    // We query the high info graph
    // It might happen that the high info graph has branches between the original
    // source and target, but we ignore them with approach.

    if(m_verbose) std::cout << "COMPUTING SHORTEST_PATH...." << std::endl;
    // compute shortest path in high info graph
    auto shortest_path =
      SG::compute_shortest_path(
          m_result_to_original_vertex_map[sg_nodes[0]][1],
          m_result_to_original_vertex_map[sg_nodes[1]][1],
          m_graphs[1].get(), m_verbose);
    if(m_verbose) {
      if(shortest_path.empty()) std::cout << "shortest_path is EMPTY!?" << std::endl;
      for(const auto & v : shortest_path) {
        std::cout << "shortest_path. v: " << v << " ---> ";
        print_pos(std::cout, m_graphs[1].get()[v].pos);
        std::cout << std::endl;
      }
      std::vector<edge_descriptor> shortest_path_edge_descriptors;
      for(size_t i = 0; i < shortest_path.size() - 1; ++i) {
        auto edge_pair = boost::edge(shortest_path[i], shortest_path[i+1], m_graphs[1].get());
        if(!edge_pair.second) std::cout << "WHAAAT? edge doesn't exist? "<< std::endl;
        shortest_path_edge_descriptors.push_back(edge_pair.first);
      }
      for(const auto & ed : shortest_path_edge_descriptors) {
        std::cout << "edge: " << ed << " --> ";
        std::cout << input_sg[ed] << std::endl;
      }
    }
    if(m_verbose) std::cout << "CREATING EDGE FROM SHORTEST_PATH...." << std::endl;
    SpatialEdge sg_edge = SG::create_edge_from_path(shortest_path, m_graphs[1].get());
    if(m_verbose) std::cout << "EDGE FROM SHORTEST_PATH CREATED." << std::endl;

    auto any_edge_exist = boost::edge(sg_nodes[0], sg_nodes[1], m_sg);
    if(!any_edge_exist.second) {
      auto added_edge = boost::add_edge(sg_nodes[0], sg_nodes[1], sg_edge, m_sg);
    }

  }

private:
  void add_to_result_to_original_map(
      const typename ResultToOriginalVertexMap::key_type & key,
      const typename ResultToOriginalVertexMap::mapped_type::value_type & value,
      size_t graph_index) {

    const size_t size_vector = m_graphs.size();
    assert(graph_index < size_vector);
    auto vertex_search = m_result_to_original_vertex_map.find(key);
    // Initialize the vector
    bool vertex_exists = false;
    if(vertex_search != m_result_to_original_vertex_map.end()) {
      vertex_exists = true;
    }
    if(!vertex_exists) {
      std::vector<vertex_descriptor> descriptors(size_vector, vertex_descriptor(-1));
      descriptors[graph_index] = value;
      m_result_to_original_vertex_map.emplace(key, descriptors);
    } else {
      // Alias, modify descriptors modifies the map
      auto & descriptors = vertex_search->second;
      if(descriptors[graph_index] != vertex_descriptor(-1)) {
        std::cout << "ALREADY ADDED u->m_result_to_original_vertex_map[u][graph_index]: "
          << key <<"->" <<m_result_to_original_vertex_map[key][graph_index] << std::endl;
      } else {
        descriptors[graph_index] = value;
      }
    }
  }
};
}  // namespace SG
#endif

