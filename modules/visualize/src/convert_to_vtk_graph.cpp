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

vtkSmartPointer<vtkMutableUndirectedGraph>
convert_to_vtk_graph(const GraphType &sg) {
    vtkSmartPointer<vtkMutableUndirectedGraph> vtk_graph =
            vtkSmartPointer<vtkMutableUndirectedGraph>::New();

    using vertex_descriptor = boost::graph_traits<GraphType>::vertex_descriptor;
    using vertex_iterator = boost::graph_traits<GraphType>::vertex_iterator;
    using edge_iterator = boost::graph_traits<GraphType>::edge_iterator;

    using VertexMap = std::unordered_map<vertex_descriptor, vtkIdType>;
    VertexMap vertex_map;
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
        auto &sg_edge_points = sg_edge.edge_points;
        auto vtk_edge =
                vtk_graph->AddEdge(vertex_map[source], vertex_map[target]);
        for (const auto &p : sg_edge_points) {
            vtk_graph->AddEdgePoint(vtk_edge.Id, p[0], p[1], p[2]);
        }
    }
    return vtk_graph;
}
} // namespace SG
