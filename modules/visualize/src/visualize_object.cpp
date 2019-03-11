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

#include "visualize_object.hpp"

#include <vtkActor.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>
#include "visualize_common.hpp"

#include <vtkInteractorStyleTrackballCameraGraph.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
namespace SG {

vtkSmartPointer<vtkPoints>
points_from_object(const DGtal::Z3i::Object26_6 & obj)
{
  auto points = vtkSmartPointer<vtkPoints>::New();
  for(const auto &p : obj.pointSet()) {
    points->InsertNextPoint(p[0], p[1], p[2]);
  }
  return points;
}

vtkSmartPointer<vtkActor>
create_actor_visualize_object(const DGtal::Z3i::Object26_6 & obj,
        const double inputOpacity)
{
  auto points = points_from_object(obj);
  return create_actor_visualize_points_as_cubes(points, inputOpacity);
}

void
visualize_object(const DGtal::Z3i::Object26_6 & obj,
        const double inputOpacity)
{
  auto actor = create_actor_visualize_object(obj, inputOpacity);

  // Create a renderer, render window, and interactor
  auto renderer = vtkSmartPointer<vtkRenderer>::New();
  auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  auto renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
  auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); // like paraview
  renderWindowInteractor->SetInteractorStyle(style);

  // Add the actor to the scene
  renderer->AddActor(actor);

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();
}


} // end namespace
