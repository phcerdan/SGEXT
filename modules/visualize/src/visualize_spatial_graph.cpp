/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "visualize_spatial_graph.hpp"

#include "visualize_common.hpp"
#include "vtkInteractorStyleTrackballCameraGraph.h"
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkPoints.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include "convert_to_vtk_graph.hpp"
namespace SG {

vtkSmartPointer<vtkGraphLayoutView>
create_graph_layout_view_from_spatial_graph(const GraphType &sg) {
    vtkSmartPointer<vtkMutableUndirectedGraph> vtk_graph =
            convert_to_vtk_graph(sg);

    // Render:
    auto graphLayoutView = vtkSmartPointer<vtkGraphLayoutView>::New();
    graphLayoutView->AddRepresentationFromInput(vtk_graph);
    graphLayoutView->SetLayoutStrategyToPassThrough();
    auto style = vtkSmartPointer<
            vtkInteractorStyleTrackballCamera>::New(); // like
                                                       // paraview
    graphLayoutView->SetInteractorStyle(style);
    graphLayoutView->SetColorVertices(true);
    // graphLayoutView->SetVertexLabelVisibility(true);
    // graphLayoutView->SetHideVertexLabelsOnInteraction(true);
    // graphLayoutView->SetGlyphType(7);

    // Flip camera because VTK-ITK different corner for origin.
    flip_camera(graphLayoutView->GetRenderer()->GetActiveCamera());

    graphLayoutView->ResetCamera();
    return graphLayoutView;
}

void visualize_spatial_graph(const GraphType &sg) {
    auto graphLayoutView = create_graph_layout_view_from_spatial_graph(sg);
    graphLayoutView->Render();
    graphLayoutView->GetInteractor()->Start();
}

void visualize_spatial_graph_with_points(const GraphType &sg,
                                         vtkPoints *points,
                                         const double pointsOpacity,
                                         const double lengthX,
                                         const double lengthY,
                                         const double lengthZ) {
    // Create a renderer, and a InteractorStyle to add to the renderWindow
    // created from the graphLayoutView.
    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    auto style = vtkSmartPointer<
            vtkInteractorStyleTrackballCameraGraph>::New(); // like paraview

    // Add actor for points
    auto pointsActor = create_actor_visualize_points_as_cubes(
            points, pointsOpacity, lengthX, lengthY, lengthZ);
    renderer->AddActor(pointsActor);
    renderer->ResetCamera();

    // TODO maybe think about using a GraphItem in a Context instead of the
    // graphLayoutView
    auto graphLayoutView = create_graph_layout_view_from_spatial_graph(sg);
    // graphLayoutView provides our window and interactor.
    auto renderWindowInteractor = graphLayoutView->GetInteractor();
    renderWindowInteractor->SetInteractorStyle(style);
    auto renderWindow = graphLayoutView->GetRenderWindow();
    renderWindow->SetNumberOfLayers(2);
    graphLayoutView->GetRenderer()->SetLayer(1);
    graphLayoutView->GetRenderer()->InteractiveOff();
    graphLayoutView->GetRenderer()->SetActiveCamera(
            renderer->GetActiveCamera());
    // Don't make the z buffer transparent of the graph layout
    graphLayoutView->GetRenderer()->EraseOff();

    renderWindow->AddRenderer(renderer);

    renderWindowInteractor->Start();
}
} // namespace SG
