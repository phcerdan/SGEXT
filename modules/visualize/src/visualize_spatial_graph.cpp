/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "visualize_spatial_graph.hpp"

#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include "vtkInteractorStyleTrackballCameraGraph.h"
#include <vtkMutableUndirectedGraph.h>
#include <vtkPoints.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include "visualize_common.hpp"

#include "convert_to_vtk_graph.hpp"
namespace SG {

vtkSmartPointer<vtkGraphLayoutView>
create_graph_layout_view_from_spatial_graph(const GraphType & sg)
{
    vtkSmartPointer<vtkMutableUndirectedGraph> vtk_graph =
        convert_to_vtk_graph(sg);

    // Render:
    auto graphLayoutView = vtkSmartPointer<vtkGraphLayoutView>::New();
    graphLayoutView->AddRepresentationFromInput(vtk_graph);
    graphLayoutView->SetLayoutStrategyToPassThrough();
    auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); // like paraview
    graphLayoutView->SetInteractorStyle(style) ;
    graphLayoutView->SetColorVertices(true);
    // graphLayoutView->SetGlyphType(7);

    // Flip camera because VTK-ITK different corner for origin.
    flip_camera(graphLayoutView->GetRenderer()->GetActiveCamera());

    graphLayoutView->ResetCamera();
    // graphLayoutView->Render();
    // graphLayoutView->GetInteractor()->Start();
    return graphLayoutView;
}

void
visualize_spatial_graph(const GraphType & sg)
{
    auto graphLayoutView = create_graph_layout_view_from_spatial_graph(sg);
    graphLayoutView->Render();
    graphLayoutView->GetInteractor()->Start();
}

void
visualize_spatial_graph_with_points(const GraphType & sg,
    vtkPoints * points,
    const double pointsOpacity)
{

    // Create a renderer, render window, and interactor
    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    auto renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    auto style = vtkSmartPointer<vtkInteractorStyleTrackballCameraGraph>::New(); // like paraview
    renderWindowInteractor->SetInteractorStyle(style);

    // Add actor for points
    auto pointsActor = create_actor_visualize_points_as_cubes(points, pointsOpacity);
    renderer->AddActor(pointsActor);

    // TODO maybe think about using a GraphItem in a Context instead of the graphLayoutView
    auto graphLayoutView = create_graph_layout_view_from_spatial_graph(sg);
    graphLayoutView->GetRenderer()->SetLayer(1);
    graphLayoutView->GetRenderer()->InteractiveOff();
    graphLayoutView->GetRenderer()->SetActiveCamera( renderer->GetActiveCamera() );
    // Don't make the z buffer transparent of the graph layout
    graphLayoutView->GetRenderer()->EraseOff();

    renderWindow->SetNumberOfLayers(2);

    renderWindow->AddRenderer(graphLayoutView->GetRenderer());
    renderer->ResetCamera();
    renderWindowInteractor->Start();
}
} // end namespace
