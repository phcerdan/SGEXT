#ifndef VISUALIZE_SPATIAL_GRAPH_HPP
#define VISUALIZE_SPATIAL_GRAPH_HPP
#include <vtkGraphLayoutView.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>

#include "convert_to_vtk_graph.hpp"

namespace SG {

template <typename SpatialGraph>
void visualize_spatial_graph(const SpatialGraph & sg)
{
    vtkSmartPointer<vtkMutableUndirectedGraph> vtk_graph =
        convert_to_vtk_graph(sg);

    // Render:
    auto graphLayoutView = vtkSmartPointer<vtkGraphLayoutView>::New();
    graphLayoutView->AddRepresentationFromInput(vtk_graph);
    graphLayoutView->SetLayoutStrategyToPassThrough();
    auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); // like paraview
    graphLayoutView->SetInteractorStyle(style) ;
    graphLayoutView->ScaledGlyphsOn();

    graphLayoutView->ResetCamera();
    graphLayoutView->Render();
    graphLayoutView->GetInteractor()->Start();
}

} // namespace SG
#endif
