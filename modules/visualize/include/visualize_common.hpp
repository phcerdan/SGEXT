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
