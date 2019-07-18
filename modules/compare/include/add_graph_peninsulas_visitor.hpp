/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ADD_GRAPH_PENINSULAS_VISITOR_HPP
#define ADD_GRAPH_PENINSULAS_VISITOR_HPP

#include "array_utilities.hpp"
#include "get_vtk_points_from_graph.hpp"
#include "graph_descriptor.hpp"
#include "graph_points_locator.hpp"
#include "shortest_path.hpp"
#include "spatial_graph_utilities.hpp"
#include <algorithm>
#include <boost/graph/adjacency_iterator.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <iostream>
#include <tuple>
#include <vtkIdList.h>
#include <vtkOctreePointLocator.h>

namespace SG {

/**
 *
 * @tparam SpatialGraph
 * @tparam VertexMap
 * @tparam ColorMap
 */
template <typename SpatialGraph, typename VertexMap, typename ColorMap>
struct AddGraphPeninsulasVisitor : public boost::default_dfs_visitor {
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

    AddGraphPeninsulasVisitor(
            SpatialGraph &result_g,
            const std::vector<std::reference_wrapper<const SpatialGraph>>
                    &graphs,
            IdGraphDescriptorMap &point_id_graphs_map,
            vtkOctreePointLocator *octree,
            double &radius,
            ColorMap &color_map,
            VertexMap &vertex_map,
            bool &verbose)
            : m_result_g(result_g), m_graphs(graphs),
              m_point_id_graphs_map(point_id_graphs_map), m_octree(octree),
              m_radius(radius), m_color_map(color_map),
              m_vertex_map(vertex_map), m_verbose(verbose) {}

    /// The resulting graph
    SpatialGraph &m_result_g;
    /// The array of graphs ordered from low to high info.
    const std::vector<std::reference_wrapper<const SpatialGraph>> &m_graphs;
    IdGraphDescriptorMap &m_point_id_graphs_map;
    vtkOctreePointLocator *m_octree;
    double &m_radius;
    /// color map to handle which nodes have been visited
    ColorMap &m_color_map;
    /** vertex_descriptor map between input (low-info) graph and the resulting
     * graph */
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
        if (m_verbose) {
            std::cout << "AddGraphPeninsula. discover_vertex: " << u << " : "
                      << ArrayUtilities::to_string(input_sg[u].pos)
                      << ". Degree:" << degree << std::endl;
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

        if (m_verbose) {
            std::cout << "AddGraphPeninsula. tree_edge: " << e
                      << " , target: " << target << " : "
                      << ArrayUtilities::to_string(input_sg[target].pos)
                      << std::endl;
        }
    }
};
} // namespace SG
#endif
