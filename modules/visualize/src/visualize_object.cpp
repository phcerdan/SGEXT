/* Copyright (C) 2018 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "visualize_object.hpp"

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3DMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
namespace SG {

vtkSmartPointer<vtkRenderWindowInteractor>
visualize_points_as_cubes(/* const */ vtkPoints * points,
        const double inputOpacity)
{
  vtkSmartPointer<vtkPolyData> polydata =
    vtkSmartPointer<vtkPolyData>::New();
  polydata->SetPoints(points);

  vtkSmartPointer<vtkPolyData> glyph =
    vtkSmartPointer<vtkPolyData>::New();

  // Create anything you want here, we will use a cube for the demo.
  vtkSmartPointer<vtkCubeSource> cubeSource =
    vtkSmartPointer<vtkCubeSource>::New();
  cubeSource->SetXLength(1.0);
  cubeSource->SetYLength(1.0);
  cubeSource->SetZLength(1.0);

  vtkSmartPointer<vtkGlyph3DMapper> glyph3Dmapper =
    vtkSmartPointer<vtkGlyph3DMapper>::New();
  glyph3Dmapper->SetSourceConnection(cubeSource->GetOutputPort());
  glyph3Dmapper->SetInputData(polydata);
  glyph3Dmapper->Update();

  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->GetProperty()->SetOpacity(inputOpacity);
  actor->SetMapper(glyph3Dmapper);

  // Create a renderer, render window, and interactor
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);
  auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); // like paraview
  renderWindowInteractor->SetInteractorStyle(style);

  // Add the actor to the scene
  renderer->AddActor(actor);

  // Render and interact
  renderWindow->Render();
  // renderWindowInteractor->Start();
  return renderWindowInteractor;
}

vtkSmartPointer<vtkRenderWindowInteractor>
visualize_object(const DGtal::Z3i::Object26_6 & obj,
        const double inputOpacity)
{
  vtkSmartPointer<vtkPoints> points =
    vtkSmartPointer<vtkPoints>::New();
  for(const auto &p : obj.pointSet()) {
    points->InsertNextPoint(p[0], p[1], p[2]);
  }
  return visualize_points_as_cubes(points, inputOpacity);
}


} // end namespace
