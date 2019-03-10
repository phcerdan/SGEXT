/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "visualize_spatial_graph.hpp"

#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkPoints.h>
#include <vtkRenderWindowInteractor.h>
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

} // end namespace
