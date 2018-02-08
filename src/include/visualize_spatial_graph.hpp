#ifndef VISUALIZE_SPATIAL_GRAPH_HPP
#define VISUALIZE_SPATIAL_GRAPH_HPP
#include <unordered_map>
#include <vtkGraphLayoutView.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <boost/graph/adjacency_iterator.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

namespace SG {
template <typename SpatialGraph>
void visualize_spatial_graph(const SpatialGraph & sg)
{
    vtkSmartPointer<vtkMutableUndirectedGraph> vtk_graph =
        vtkSmartPointer<vtkMutableUndirectedGraph>::New();

    using vertex_descriptor =
        typename boost::graph_traits<SpatialGraph>::vertex_descriptor;
    using edge_descriptor =
        typename boost::graph_traits<SpatialGraph>::edge_descriptor;
    using vertex_iterator =
        typename boost::graph_traits<SpatialGraph>::vertex_iterator;
    using edge_iterator =
        typename boost::graph_traits<SpatialGraph>::edge_iterator;

    using VertexMap = std::unordered_map<vertex_descriptor, vtkIdType>;
    VertexMap vertex_map;
    // To put coordinates in nodes (the default is 0,0,0)
    auto points = vtkSmartPointer<vtkPoints>::New();

    vertex_iterator vi, vi_end;
    std::tie(vi, vi_end) = boost::vertices(sg);
    for (; vi != vi_end; ++vi) {
        vertex_map[*vi] = vtk_graph->AddVertex();
        points->InsertNextPoint(sg[*vi].pos[0],
                                sg[*vi].pos[1],
                                sg[*vi].pos[2]);
    }
    vtk_graph->SetPoints(points);

    edge_iterator ei, ei_end;
    std::tie(ei, ei_end) = boost::edges(sg);
    for (; ei != ei_end; ++ei) {
        auto source = boost::source(*ei, sg);
        auto target = boost::target(*ei, sg);
        auto &sg_edge = sg[*ei];
        auto &sg_edge_points = sg_edge.edge_points;
        auto vtk_edge = vtk_graph->AddEdge(vertex_map[source], vertex_map[target]);
        for(const auto& p : sg_edge_points){
            vtk_graph->AddEdgePoint(vtk_edge.Id, p[0],p[1],p[2]);
        }
    }
    // Render:
    auto graphLayoutView = vtkSmartPointer<vtkGraphLayoutView>::New();
    graphLayoutView->AddRepresentationFromInput(vtk_graph);
    graphLayoutView->SetLayoutStrategyToPassThrough();
    auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); // like paraview
    graphLayoutView->SetInteractorStyle(style) ;

    graphLayoutView->ResetCamera();
    graphLayoutView->Render();
    graphLayoutView->GetInteractor()->Start();
}
} // namespace SG
#endif
