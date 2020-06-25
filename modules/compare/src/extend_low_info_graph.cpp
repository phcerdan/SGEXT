/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "extend_low_info_graph.hpp"
#include "extend_low_info_graph_visitor.hpp"
#include <tuple> // For std::tie

namespace SG {

/**
 * Extend the low info graph using high info graphs
 *
 * See @ref extend_low_info_graph_visitor
 *
 * @param graphs the first graph is the low info graph
 * @param idMap map between vtk and graph_descriptors
 * @param octree locator
 * @param radius for octree
 * @param verbose
 *
 * @return extended low info graph
 */
GraphType extend_low_info_graph_via_dfs(
        const std::vector<std::reference_wrapper<const GraphType>> &graphs,
        std::unordered_map<vtkIdType, std::vector<graph_descriptor>> &idMap,
        vtkOctreePointLocator *octree,
        double radius,
        bool verbose) {
    const GraphType &input_sg = graphs[0];
    GraphType result_sg;
    using vertex_descriptor = boost::graph_traits<GraphType>::vertex_descriptor;
    using vertex_iterator = boost::graph_traits<GraphType>::vertex_iterator;

    using ColorMap = std::map<vertex_descriptor, boost::default_color_type>;
    ColorMap colorMap;
    using Color = boost::color_traits<ColorMap::mapped_type>;
    boost::associative_property_map<ColorMap> propColorMap(colorMap);

    using VertexMap = std::unordered_map<vertex_descriptor, vertex_descriptor>;
    VertexMap vertex_map;

    ExtendLowInfoGraphVisitor<GraphType, VertexMap, ColorMap> vis(
            result_sg, graphs, idMap, octree, radius, colorMap, vertex_map,
            verbose);

    // Mark as unvisited (white) all the vertices
    vertex_iterator ui, ui_end;
    for (std::tie(ui, ui_end) = boost::vertices(input_sg); ui != ui_end; ++ui) {
        put(propColorMap, *ui, Color::white());
    }

    vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(input_sg);
    auto start = *vi;

    for (; vi != vi_end; ++vi) {
        auto degree = boost::out_degree(*vi, input_sg);
        start = *vi;
        if (verbose)
            std::cout << "ExtendLowInfoGraphVisitor Visit: start: " << start
                      << " : " << ArrayUtilities::to_string(input_sg[start].pos)
                      << ". Degree: " << degree << std::endl;
        boost::depth_first_visit(input_sg, start, vis, propColorMap);
    }

    return result_sg;
};

} // end namespace SG
