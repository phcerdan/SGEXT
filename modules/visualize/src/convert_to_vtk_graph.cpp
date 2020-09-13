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

#include "convert_to_vtk_graph.hpp"
#include <unordered_map>

#include <vtkMutableUndirectedGraph.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

namespace SG {

GraphVTK convert_to_vtk_graph(const GraphType &sg) {
    GraphVTK output;
    auto & vtk_graph = output.vtk_graph;

    using vertex_descriptor = boost::graph_traits<GraphType>::vertex_descriptor;
    using vertex_iterator = boost::graph_traits<GraphType>::vertex_iterator;
    using edge_iterator = boost::graph_traits<GraphType>::edge_iterator;

    auto & vertex_map = output.vertex_map;
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
    return output;
}
} // namespace SG
