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

#include <vtkGraphLayoutView.h>
#include <vtkMutableUndirectedGraph.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>

#include "convert_to_vtk_graph.hpp"
namespace SG {

void visualize_spatial_graph(const GraphType & sg)
{
    vtkSmartPointer<vtkMutableUndirectedGraph> vtk_graph =
        convert_to_vtk_graph(sg);

    // Render:
    auto graphLayoutView = vtkSmartPointer<vtkGraphLayoutView>::New();
    graphLayoutView->AddRepresentationFromInput(vtk_graph);
    graphLayoutView->SetLayoutStrategyToPassThrough();
    // auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); // like paraview
    // graphLayoutView->SetInteractorStyle(style) ;
    graphLayoutView->SetColorVertices(true);

    // Flip camera because VTK-ITK different corner for origin.
    double pos[3];
    double vup[3];
    vtkCamera *cam = graphLayoutView->GetRenderer()->GetActiveCamera();
    cam->GetPosition(pos);
    cam->GetViewUp(vup);
    for ( unsigned int i = 0; i < 3; ++i )
    {
        pos[i] = -pos[i];
        vup[i] = -vup[i];
    }
    cam->SetPosition(pos);
    cam->SetViewUp(vup);

    graphLayoutView->ResetCamera();
    graphLayoutView->Render();
    graphLayoutView->GetInteractor()->Start();
}

} // end namespace
