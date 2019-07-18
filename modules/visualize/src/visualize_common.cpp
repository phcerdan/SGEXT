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

#include "visualize_common.hpp"
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3DMapper.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>

namespace SG {

void flip_camera(vtkCamera *cam) {
    double pos[3];
    double vup[3];
    cam->GetPosition(pos);
    cam->GetViewUp(vup);
    for (unsigned int i = 0; i < 3; ++i) {
        pos[i] = -pos[i];
        vup[i] = -vup[i];
    }
    cam->SetPosition(pos);
    cam->SetViewUp(vup);
}

vtkSmartPointer<vtkActor> create_actor_visualize_points_as_cubes(
        /* const */ vtkPoints *points,
        const double inputOpacity,
        const double lengthX,
        const double lengthY,
        const double lengthZ) {
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);

    vtkSmartPointer<vtkPolyData> glyph = vtkSmartPointer<vtkPolyData>::New();

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

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->GetProperty()->SetOpacity(inputOpacity);
    actor->SetMapper(glyph3Dmapper);

    return actor;
}

} // namespace SG
