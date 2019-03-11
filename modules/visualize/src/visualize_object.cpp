/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
