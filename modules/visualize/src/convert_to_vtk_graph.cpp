/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "convert_to_vtk_graph.hpp"
#include <unordered_map>

#include <vtkMutableUndirectedGraph.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

namespace SG {

GraphVTK convert_to_vtk_graph(const GraphType &sg, const bool & with_edge_points) {
    GraphVTK output;
    auto &vtk_graph = output.vtk_graph;

    using vertex_descriptor = boost::graph_traits<GraphType>::vertex_descriptor;
    using vertex_iterator = boost::graph_traits<GraphType>::vertex_iterator;
    using edge_iterator = boost::graph_traits<GraphType>::edge_iterator;

    auto &vertex_map = output.vertex_map;
    // To put coordinates in nodes (the default is 0,0,0)
    auto points = vtkSmartPointer<vtkPoints>::New();

    vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(sg);
    for (; vi != vi_end; ++vi) {
        vertex_map[*vi] = vtk_graph->AddVertex();
        points->InsertNextPoint(sg[*vi].pos[0], sg[*vi].pos[1], sg[*vi].pos[2]);
    }
    vtk_graph->SetPoints(points);

    edge_iterator ei, ei_end;
    std::tie(ei, ei_end) = boost::edges(sg);
    for (; ei != ei_end; ++ei) {
        auto source = boost::source(*ei, sg);
        auto target = boost::target(*ei, sg);
        auto &sg_edge = sg[*ei];
        const auto &sg_edge_points = sg_edge.edge_points;
        auto vtk_edge =
                vtk_graph->AddEdge(vertex_map.at(source), vertex_map.at(target));
        if(with_edge_points) {
            // Check if the first point in edge_points, is closer to source, or to
            // target. Only do the check if there is at least one edge_point.
            const bool source_is_closer_to_begin =
                sg_edge_points.empty()
                ? true
                : ArrayUtilities::distance(sg[source].pos,
                        sg_edge_points[0]) <
                ArrayUtilities::distance(sg[target].pos,
                        sg_edge_points[0]);
            if (source_is_closer_to_begin) {
                for (const auto &p : sg_edge_points) {
                    vtk_graph->AddEdgePoint(vtk_edge.Id, p[0], p[1], p[2]);
                }
            }
            else {
                for (auto it = sg_edge_points.crbegin(); it != sg_edge_points.crend();
                        ++it) {
                    const auto &p = *it;
                    vtk_graph->AddEdgePoint(vtk_edge.Id, p[0], p[1], p[2]);
                }
            }
        }
    }
    return output;
}
} // namespace SG
