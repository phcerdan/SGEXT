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

void visualize_spatial_graph_with_points(
        const GraphType &sg,
        vtkPoints *points,
        const double pointsOpacity,
        const std::array<double, 3> cube_length) {
    // Create a renderer, and a InteractorStyle to add to the renderWindow
    // created from the graphLayoutView.
    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    auto style = vtkSmartPointer<
            vtkInteractorStyleTrackballCameraGraph>::New(); // like paraview

    // Add actor for points
    auto pointsActor = create_actor_visualize_points_as_cubes(
            points, pointsOpacity, cube_length);
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
