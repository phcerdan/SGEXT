/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "visualize_common.hpp"
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3DMapper.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>

namespace SG {

void flip_camera(vtkCamera * cam)
{
    double pos[3];
    double vup[3];
    cam->GetPosition(pos);
    cam->GetViewUp(vup);
    for ( unsigned int i = 0; i < 3; ++i )
    {
        pos[i] = -pos[i];
        vup[i] = -vup[i];
    }
    cam->SetPosition(pos);
    cam->SetViewUp(vup);
}

vtkSmartPointer<vtkActor>
create_actor_visualize_points_as_cubes(
    /* const */ vtkPoints * points,
        const double inputOpacity,
        const double lengthX,
        const double lengthY,
        const double lengthZ
        )
{
  vtkSmartPointer<vtkPolyData> polydata =
    vtkSmartPointer<vtkPolyData>::New();
  polydata->SetPoints(points);

  vtkSmartPointer<vtkPolyData> glyph =
    vtkSmartPointer<vtkPolyData>::New();

  // Create anything you want here, we will use a cube for the demo.
  vtkSmartPointer<vtkCubeSource> cubeSource =
    vtkSmartPointer<vtkCubeSource>::New();
  cubeSource->SetXLength(lengthX);
  cubeSource->SetYLength(lengthY);
  cubeSource->SetZLength(lengthZ);

  vtkSmartPointer<vtkGlyph3DMapper> glyph3Dmapper =
    vtkSmartPointer<vtkGlyph3DMapper>::New();
  glyph3Dmapper->SetSourceConnection(cubeSource->GetOutputPort());
  glyph3Dmapper->SetInputData(polydata);
  glyph3Dmapper->Update();

  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->GetProperty()->SetOpacity(inputOpacity);
  actor->SetMapper(glyph3Dmapper);

  return actor;
}


} // end ns SG
