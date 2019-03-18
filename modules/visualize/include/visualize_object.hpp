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

#ifndef VISUALIZE_OBJECT_HPP
#define VISUALIZE_OBJECT_HPP

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPoints.h>
#include <DGtal/helpers/StdDefs.h>

namespace SG {

/**
 * Copy the DGtal object into vtkPoints
 *
 * @param obj
 *
 * @return
 */
vtkSmartPointer<vtkPoints> points_from_object(
    const DGtal::Z3i::Object26_6& obj);

/**
 * Create an actor to visualize the default 3D DGtal::Object
 *
 * @param obj
 * @param inputOpacity
 *
 * @return
 */
vtkSmartPointer<vtkActor> create_actor_visualize_object(
    const DGtal::Z3i::Object26_6& obj, const double inputOpacity = 0.8);

/**
 * Render object
 *
 * @ref create_actor_visualize_object
 *
 * @param obj
 * @param inputOpacity
 */
void visualize_object(const DGtal::Z3i::Object26_6& obj,
                      const double inputOpacity = 0.8);
}  // end namespace SG
#endif
