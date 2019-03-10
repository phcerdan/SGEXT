/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VISUALIZE_COMMON_HPP
#define VISUALIZE_COMMON_HPP
#include <vtkCamera.h>
#include <vtkActor.h>
#include <vtkSmartPointer.h>

namespace SG {
/**
 * Flip camera because VTK-ITK different corner for origin.
 *
 * @param cam
 */
void flip_camera(vtkCamera *cam);

/**
 * Create an actor to visualize points as cubes.
 *
 * @param points
 * @param inputOpacity
 *
 * @return
 */
vtkSmartPointer<vtkActor>
create_actor_visualize_points_as_cubes( /* const */ vtkPoints * points,
    const double inputOpacity);


} // end ns SG
#endif
